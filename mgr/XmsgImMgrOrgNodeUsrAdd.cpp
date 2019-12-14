/*
  Copyright 2019 www.dev5.cn, Inc. dev5@qq.com
 
  This file is part of X-MSG-IM.
 
  X-MSG-IM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  X-MSG-IM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU Affero General Public License
  along with X-MSG-IM.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <libx-msg-im-org-db.h>
#include "XmsgImMgrOrgNodeUsrAdd.h"

XmsgImMgrOrgNodeUsrAdd::XmsgImMgrOrgNodeUsrAdd()
{

}

void XmsgImMgrOrgNodeUsrAdd::handle(shared_ptr<XmsgNeUsr> nu, SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeUsrAddReq> req)
{
	auto it = req->infoaccount().kv().find("usr");
	if (it == req->infoaccount().kv().end()) 
	{
		LOG_WARN("usr is empty, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "usr is empty");
		return;
	}
	string usr = it->second;
	if (usr.empty()) 
	{
		LOG_WARN("usr is empty, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "usr is empty");
		return;
	}
	if (req->nameusr().empty())
	{
		LOG_WARN("name-usr is empty, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "name-usr is empty");
		return;
	}
	if (req->namedept().empty())
	{
		LOG_WARN("name-dept is empty, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "name-dept is empty");
		return;
	}
	SptrCgt pcgt = req->pcgt().empty() ? XmsgImOrgMgr::instance()->getRoot()->cgt : ChannelGlobalTitle::parse(req->pcgt()); 
	if (pcgt == nullptr)
	{
		LOG_WARN("parent channel global title format error, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "parent channel global title format error: %s", req->ShortDebugString().c_str());
		return;
	}
	int ret = XmsgImOrgMgr::instance()->isUsrExistInDept(pcgt->toString(), req->namedept() ); 
	if (ret == 1)
	{
		LOG_WARN("user already existed, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "name-dept already existed: %s", req->namedept().c_str());
		return;
	}
	if (ret == 2)
	{
		LOG_WARN("department is not exist, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "department is not exist: %s", req->pcgt().c_str());
		return;
	}
	XmsgImMgrOrgNodeUsrAdd::addUsrRegiste(nu, trans, req, pcgt);
}

void XmsgImMgrOrgNodeUsrAdd::addUsrRegiste(shared_ptr<XmsgNeUsr> nu, SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeUsrAddReq> req, SptrCgt pcgt)
{
	auto hlr = XmsgNeMgr::instance()->getHlr();
	if (hlr == nullptr)
	{
		LOG_ERROR("can not allocate x-msg-im-hlr, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_EXCEPTION, "can not allocate x-msg-im-hlr");
		return;
	}
	shared_ptr<XmsgImAuthOrgRegisteReq> regReq(new XmsgImAuthOrgRegisteReq());
	*(regReq->mutable_info()) = req->infoaccount().kv();
	XmsgImChannel::cast(hlr->channel)->begin(regReq, [trans, req, pcgt](SptrXiti itrans)
	{
		if(itrans->ret != RET_SUCCESS)
		{
			LOG_ERROR("registe account in x-msg-im-hlr failed, ret: %04X, desc: %s, req: %s", itrans->ret, itrans->desc.c_str(), req->ShortDebugString().c_str())
			trans->endDesc(itrans->ret, itrans->desc.c_str());
			return;
		}
		LOG_INFO("registe account in x-msg-im-hlr successful, req: %s, rsp: %s", req->ShortDebugString().c_str(), itrans->endMsg->ShortDebugString().c_str())
		XmsgImOrgDb::instance()->future([trans, req, pcgt, itrans]
				{
					XmsgImMgrOrgNodeUsrAdd::addUsr2db(trans, req, pcgt, static_pointer_cast<XmsgImAuthOrgRegisteRsp>(itrans->endMsg));
				});
	}, nullptr, trans);
}

void XmsgImMgrOrgNodeUsrAdd::addUsr2db(SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeUsrAddReq> req, SptrCgt pcgt, shared_ptr<XmsgImAuthOrgRegisteRsp> rsp)
{
	SptrCgt cgt = ChannelGlobalTitle::parse(rsp->cgt());
	if (cgt == nullptr)
	{
		LOG_FAULT("it`s a bug, can not add user node, rsp: %s", rsp->ShortDebugString().c_str())
		trans->endDesc(RET_EXCEPTION, "system exception, name-usr: %s, name-dept: %s", req->nameusr().c_str(), req->namedept().c_str());
		return;
	}
	SptrOrgUsr usr(new XmsgImOrgUsrColl());
	usr->cgt = cgt;
	usr->usr = req->infoaccount().kv().find("usr")->second; 
	usr->name = req->nameusr();
	usr->enable = req->enableusr();
	usr->info.reset(new XmsgImOrgNodeInfo());
	usr->info->CopyFrom(req->infousr());
	usr->ver = XmsgImOrgMgr::instance()->nextVer4usr(); 
	usr->gts = DateMisc::nowGmt0();
	usr->uts = usr->gts;
	SptrOrgDeptUsr deptUsr(new XmsgImOrgDeptUsrColl());
	deptUsr->dcgt = pcgt;
	deptUsr->ucgt = usr->cgt;
	deptUsr->name = req->namedept();
	deptUsr->enable = req->enabledept();
	deptUsr->info.reset(new XmsgImOrgNodeInfo());
	deptUsr->info->CopyFrom(req->infodept());
	deptUsr->ver = XmsgImOrgMgr::instance()->nextVer4deptUsr(); 
	deptUsr->gts = usr->gts;
	deptUsr->uts = deptUsr->gts;
	if (!XmsgImOrgDeptUsrCollOper::instance()->insert(usr, deptUsr))
	{
		LOG_ERROR("can not add user node, may be database exception, usr: %s, deptUsr: %s", usr->toString().c_str(), deptUsr->toString().c_str())
		trans->endDesc(RET_EXCEPTION, "can not add user node, may be database exception, name-usr: %s, name-dept: %s", req->nameusr().c_str(), req->namedept().c_str());
		return;
	}
	XmsgImOrgSubClientMgr::instance()->pubEventUsr(usr->clone() ); 
	XmsgImOrgSubClientMgr::instance()->pubEventDeptUsr(deptUsr->clone() ); 
	XmsgImOrgMgr::instance()->addOrgUsr(usr); 
	XmsgImOrgMgr::instance()->addOrgDeptUsr(deptUsr); 
	XmsgImOrgMgr::instance()->setVer4usr(usr->ver); 
	XmsgImOrgMgr::instance()->setVer4deptUsr(deptUsr->ver); 
	LOG_INFO("add user node successful, usr: %s, deptUsr: %s", usr->toString().c_str(), deptUsr->toString().c_str())
	shared_ptr<XmsgImMgrOrgNodeUsrAddRsp> rspx(new XmsgImMgrOrgNodeUsrAddRsp());
	rspx->set_cgt(usr->cgt->toString());
	trans->end(rspx);
}

XmsgImMgrOrgNodeUsrAdd::~XmsgImMgrOrgNodeUsrAdd()
{

}

