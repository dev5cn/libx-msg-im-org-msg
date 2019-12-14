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
#include "XmsgImMgrOrgNodeDeptAdd.h"

XmsgImMgrOrgNodeDeptAdd::XmsgImMgrOrgNodeDeptAdd()
{

}

void XmsgImMgrOrgNodeDeptAdd::handle(shared_ptr<XmsgNeUsr> nu, SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeDeptAddReq> req)
{
	if (req->name().empty())
	{
		LOG_WARN("name is empty, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "name is empty: %s", req->ShortDebugString().c_str());
		return;
	}
	SptrCgt pcgt = req->pcgt().empty() ? XmsgImOrgMgr::instance()->getRoot()->cgt : ChannelGlobalTitle::parse(req->pcgt()); 
	if (pcgt == nullptr)
	{
		LOG_WARN("parent channel global title format error, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "parent channel global title format error: %s", req->ShortDebugString().c_str());
		return;
	}
	int ret = XmsgImOrgMgr::instance()->isDeptExistInPartent(pcgt->toString(), req->name() ); 
	if (ret == 1) 
	{
		LOG_WARN("department already existed, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "department already existed: %s", req->name().c_str());
		return;
	}
	if (ret == 2) 
	{
		LOG_WARN("parent department is not exist, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORMAT_ERROR, "parent department is not exist: %s", req->pcgt().c_str());
		return;
	}
	XmsgImOrgDb::instance()->future([nu, trans, req, pcgt]
	{
		XmsgImMgrOrgNodeDeptAdd::addDept(nu, trans, req, pcgt);
	});
}

void XmsgImMgrOrgNodeDeptAdd::addDept(shared_ptr<XmsgNeUsr> nu, SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeDeptAddReq> req, SptrCgt pcgt)
{
	SptrOrgDept coll(new XmsgImOrgDeptColl());
	coll->cgt = ChannelGlobalTitle::genGroup(XmsgImOrgCfg::instance()->cgt); 
	coll->pcgt = pcgt;
	coll->name = req->name();
	coll->info.reset(new XmsgImOrgNodeInfo());
	coll->info->CopyFrom(req->info());
	coll->enable = req->enable();
	coll->ver = XmsgImOrgMgr::instance()->nextVer4dept(); 
	coll->gts = DateMisc::nowGmt0();
	coll->uts = coll->gts;
	if (!XmsgImOrgDeptCollOper::instance()->insert(coll))
	{
		trans->endDesc(RET_EXCEPTION, "can not add department node, may be database exception, name: %s", req->name().c_str());
		LOG_ERROR("can not add department node, may be database exception, coll: %s", coll->toString().c_str())
		return;
	}
	XmsgImOrgSubClientMgr::instance()->pubEventDept(coll->clone() ); 
	XmsgImOrgMgr::instance()->addOrgDept(coll); 
	XmsgImOrgMgr::instance()->setVer4dept(coll->ver); 
	LOG_INFO("add department node successful, coll: %s", coll->toString().c_str())
	shared_ptr<XmsgImMgrOrgNodeDeptAddRsp> rsp(new XmsgImMgrOrgNodeDeptAddRsp());
	rsp->set_cgt(coll->cgt->toString());
	trans->end(rsp);
}

XmsgImMgrOrgNodeDeptAdd::~XmsgImMgrOrgNodeDeptAdd()
{

}

