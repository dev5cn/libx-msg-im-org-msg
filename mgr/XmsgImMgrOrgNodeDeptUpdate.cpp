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
#include "XmsgImMgrOrgNodeDeptUpdate.h"

XmsgImMgrOrgNodeDeptUpdate::XmsgImMgrOrgNodeDeptUpdate()
{

}

void XmsgImMgrOrgNodeDeptUpdate::handle(shared_ptr<XmsgNeUsr> nu, SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeDeptUpdateReq> req)
{
	if (req->cgt().empty())
	{
		trans->endDesc(RET_FORMAT_ERROR, "channel global title format error: %s", req->cgt().c_str());
		return;
	}
	if (ChannelGlobalTitle::parse(req->cgt()) == nullptr)
	{
		trans->endDesc(RET_FORMAT_ERROR, "channel global title format error: %s", req->cgt().c_str());
		return;
	}
	if (req->name().empty() && !req->has_upsert() && req->remove().empty())
	{
		trans->endDesc(RET_FORBIDDEN, "update what?");
		return;
	}
	SptrOrgDept dept = XmsgImOrgMgr::instance()->findDept(req->cgt());
	if (dept == nullptr)
	{
		trans->endDesc(RET_NOT_FOUND, "can not found department for channel global title: %s", req->cgt().c_str());
		return;
	}
	XmsgImOrgDb::instance()->future([nu, trans, req, dept]
	{
		XmsgImMgrOrgNodeDeptUpdate::update2db(nu, trans, req, dept->clone() );
	});
}

void XmsgImMgrOrgNodeDeptUpdate::update2db(shared_ptr<XmsgNeUsr> nu, SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeDeptUpdateReq> req, SptrOrgDept dept)
{
	if (!req->name().empty())
		dept->name = req->name();
	XmsgMisc::updateKv(req->upsert().kv(), req->remove(), *(dept->info->mutable_kv()));
	dept->ver = XmsgImOrgMgr::instance()->nextVer4dept();
	dept->uts = DateMisc::nowGmt0();
	if (!XmsgImOrgDeptCollOper::instance()->update(dept))
	{
		LOG_ERROR("update department info failed, may be database exception, dept: %s", dept->toString().c_str())
		trans->endDesc(RET_EXCEPTION, "update department info failed, may be database exception");
		return;
	}
	XmsgImOrgMgr::instance()->updateOrgDept(dept);
	XmsgImOrgMgr::instance()->setVer4dept(dept->ver);
	shared_ptr<XmsgImMgrOrgNodeDeptUpdateRsp> rsp(new XmsgImMgrOrgNodeDeptUpdateRsp());
	XmsgMisc::insertKv(rsp->mutable_ext(), "ok", "true");
	trans->end(rsp);
}

XmsgImMgrOrgNodeDeptUpdate::~XmsgImMgrOrgNodeDeptUpdate()
{

}

