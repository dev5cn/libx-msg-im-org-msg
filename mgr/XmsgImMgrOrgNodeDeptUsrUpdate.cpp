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
#include "XmsgImMgrOrgNodeDeptUsrUpdate.h"

XmsgImMgrOrgNodeDeptUsrUpdate::XmsgImMgrOrgNodeDeptUsrUpdate()
{

}

void XmsgImMgrOrgNodeDeptUsrUpdate::handle(shared_ptr<XmsgNeUsr> nu, SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeDeptUsrUpdateReq> req)
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
	if (req->name().empty() && !req->has_upsert() && req->remove().empty())
	{
		trans->endDesc(RET_FORBIDDEN, "update what?");
		return;
	}
	XmsgImOrgDb::instance()->future([trans, req, dcgt, ucgt]
	{
		XmsgImMgrOrgNodeDeptUsrUpdate::update2db(trans, req, dcgt, ucgt);
	});
}

void XmsgImMgrOrgNodeDeptUsrUpdate::update2db(SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeDeptUsrUpdateReq> req, SptrCgt dcgt, SptrCgt ucgt)
{
	SptrOrgDeptUsr deptUsr = XmsgImOrgMgr::instance()->findDeptUsr(req->dcgt(), req->ucgt());
	if (deptUsr == nullptr) 
	{
		LOG_DEBUG("department and user relation not existed, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_FORBIDDEN, "department and user relation note existed");
		return;
	}
	if (!req->name().empty())
		deptUsr->name = req->name();
	XmsgMisc::updateKv(req->upsert().kv(), req->remove(), *(deptUsr->info->mutable_kv()));
	deptUsr->enable = !deptUsr->enable;
	deptUsr->ver = XmsgImOrgMgr::instance()->nextVer4deptUsr(); 
	deptUsr->uts = DateMisc::nowGmt0();
	if (!XmsgImOrgDeptUsrCollOper::instance()->update(deptUsr))
	{
		LOG_ERROR("update department and user relation failed, may be database exception, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_EXCEPTION, "update department and user relation failed, may be database exception");
		return;
	}
	XmsgImOrgMgr::instance()->updateOrgDeptUsr(deptUsr->clone()); 
	XmsgImOrgMgr::instance()->setVer4deptUsr(deptUsr->ver); 
	LOG_INFO("update department and user relation successful, deptUsr: %s", deptUsr->toString().c_str())
	shared_ptr<XmsgImMgrOrgNodeDeptUsrDisableOrEnableRsp> rsp(new XmsgImMgrOrgNodeDeptUsrDisableOrEnableRsp());
	XmsgMisc::insertKv(rsp->mutable_ext(), "ok", "true");
	trans->end(rsp);
}

XmsgImMgrOrgNodeDeptUsrUpdate::~XmsgImMgrOrgNodeDeptUsrUpdate()
{

}

