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
#include "XmsgImMgrOrgNodeDeptUsrAdd.h"

XmsgImMgrOrgNodeDeptUsrAdd::XmsgImMgrOrgNodeDeptUsrAdd()
{

}

void XmsgImMgrOrgNodeDeptUsrAdd::handle(shared_ptr<XmsgNeUsr> nu, SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeDeptUsrAddReq> req)
{
	if (req->dcgt().empty())
	{
		LOG_DEBUG("department channel global title format error, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "department channel global title format error");
		return;
	}
	SptrCgt dcgt = ChannelGlobalTitle::parse(req->dcgt());
	if (dcgt == nullptr)
	{
		LOG_DEBUG("department channel global title format error, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "department channel global title format error");
		return;
	}
	if (req->ucgt().empty())
	{
		LOG_DEBUG("user channel global title format error, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "user channel global title format error");
		return;
	}
	SptrCgt ucgt = ChannelGlobalTitle::parse(req->ucgt());
	if (ucgt == nullptr)
	{
		LOG_DEBUG("user channel global title format error, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "user channel global title format error");
		return;
	}
	if (req->name().empty())
	{
		LOG_DEBUG("user name can not be null, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "user name can not be null");
		return;
	}
	XmsgImOrgDb::instance()->future([trans, req, dcgt, ucgt]
	{
		XmsgImMgrOrgNodeDeptUsrAdd::insert2db(trans, req, dcgt, ucgt);
	});
}

void XmsgImMgrOrgNodeDeptUsrAdd::insert2db(SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeDeptUsrAddReq> req, SptrCgt dcgt, SptrCgt ucgt)
{
	if (XmsgImOrgMgr::instance()->findDept(req->dcgt()) == nullptr) 
	{
		LOG_DEBUG("can not found department for channel global title, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORBIDDEN, "can not found user for channel global title");
		return;
	}
	if (XmsgImOrgMgr::instance()->findUsr(req->ucgt()) == nullptr) 
	{
		LOG_DEBUG("can not found user for channel global title, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORBIDDEN, "can not found user for channel global title");
		return;
	}
	SptrOrgDeptUsr deptUsr = XmsgImOrgMgr::instance()->findDeptUsr(req->dcgt(), req->ucgt());
	if (deptUsr != nullptr) 
	{
		LOG_DEBUG("department and user relation already existed, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORBIDDEN, "department and user relation already existed");
		return;
	}
	deptUsr.reset(new XmsgImOrgDeptUsrColl());
	deptUsr->dcgt = dcgt;
	deptUsr->ucgt = ucgt;
	deptUsr->name = req->name();
	deptUsr->enable = req->enable();
	deptUsr->info.reset(new XmsgImOrgNodeInfo());
	deptUsr->info->CopyFrom(req->info());
	deptUsr->ver = XmsgImOrgMgr::instance()->nextVer4deptUsr(); 
	deptUsr->gts = DateMisc::nowGmt0();
	deptUsr->uts = deptUsr->gts;
	if (!XmsgImOrgDeptUsrCollOper::instance()->insert(deptUsr))
	{
		LOG_ERROR("add department and user relation failed, may be database exception, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_EXCEPTION, "add department and user relation failed, may be database exception");
		return;
	}
	XmsgImOrgSubClientMgr::instance()->pubEventDeptUsr(deptUsr->clone() ); 
	XmsgImOrgMgr::instance()->addOrgDeptUsr(deptUsr); 
	XmsgImOrgMgr::instance()->setVer4deptUsr(deptUsr->ver); 
	LOG_INFO("add department and user relation successful, deptUsr: %s", deptUsr->toString().c_str())
	shared_ptr<XmsgImMgrOrgNodeDeptUsrAddRsp> rsp(new XmsgImMgrOrgNodeDeptUsrAddRsp());
	XmsgMisc::insertKv(rsp->mutable_ext(), "ok", "true");
	trans->end(rsp);
}

XmsgImMgrOrgNodeDeptUsrAdd::~XmsgImMgrOrgNodeDeptUsrAdd()
{

}

