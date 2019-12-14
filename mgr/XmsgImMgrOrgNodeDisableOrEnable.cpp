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
#include "XmsgImMgrOrgNodeDisableOrEnable.h"

XmsgImMgrOrgNodeDisableOrEnable::XmsgImMgrOrgNodeDisableOrEnable()
{

}

void XmsgImMgrOrgNodeDisableOrEnable::handle(shared_ptr<XmsgNeUsr> nu, SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeDisableOrEnableReq> req)
{
	if (req->cgt().empty())
	{
		trans->endDesc(RET_FORMAT_ERROR, "channel global title format error: %s", req->cgt().c_str());
		return;
	}
	SptrCgt cgt = ChannelGlobalTitle::parse(req->cgt());
	if (cgt == nullptr)
	{
		trans->endDesc(RET_FORMAT_ERROR, "channel global title format error: %s", req->cgt().c_str());
		return;
	}
	XmsgImOrgDb::instance()->future([trans, req, cgt]
	{
		if (cgt->isGroup())
		{
			XmsgImMgrOrgNodeDisableOrEnable::update2db4dept(trans, req, cgt);
			return;
		}
		XmsgImMgrOrgNodeDisableOrEnable::update2db4usr(trans, req, cgt);
	});
}

void XmsgImMgrOrgNodeDisableOrEnable::update2db4dept(SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeDisableOrEnableReq> req, SptrCgt cgt)
{
	SptrOrgDept dept = XmsgImOrgMgr::instance()->findDept(req->cgt());
	if (dept == nullptr)
	{
		trans->endDesc(RET_NOT_FOUND, "can not found department for channel global title: %s", req->cgt().c_str());
		return;
	}
	dept->enable = !dept->enable;
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
	shared_ptr<XmsgImMgrOrgNodeDisableOrEnableRsp> rsp(new XmsgImMgrOrgNodeDisableOrEnableRsp());
	XmsgMisc::insertKv(rsp->mutable_ext(), "ok", "true");
	trans->end(rsp);
}

void XmsgImMgrOrgNodeDisableOrEnable::update2db4usr(SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeDisableOrEnableReq> req, SptrCgt cgt)
{
	SptrOrgUsr usr = XmsgImOrgMgr::instance()->findUsr(req->cgt());
	if (usr == nullptr)
	{
		trans->endDesc(RET_NOT_FOUND, "can not found user for channel global title: %s", req->cgt().c_str());
		return;
	}
	usr->enable = !usr->enable;
	usr->ver = XmsgImOrgMgr::instance()->nextVer4usr(); 
	usr->uts = DateMisc::nowGmt0();
	if (!XmsgImOrgUsrCollOper::instance()->update(usr))
	{
		LOG_ERROR("update user info failed, may be database exception, user: %s", usr->toString().c_str())
		trans->endDesc(RET_EXCEPTION, "update user info failed, may be database exception");
		return;
	}
	XmsgImOrgMgr::instance()->updateOrgUsr(usr);
	XmsgImOrgMgr::instance()->setVer4usr(usr->ver);
	shared_ptr<XmsgImMgrOrgNodeDisableOrEnableRsp> rsp(new XmsgImMgrOrgNodeDisableOrEnableRsp());
	XmsgMisc::insertKv(rsp->mutable_ext(), "ok", "true");
	trans->end(rsp);
}

XmsgImMgrOrgNodeDisableOrEnable::~XmsgImMgrOrgNodeDisableOrEnable()
{

}

