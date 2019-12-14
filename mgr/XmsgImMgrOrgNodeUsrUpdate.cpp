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
#include "XmsgImMgrOrgNodeUsrUpdate.h"

XmsgImMgrOrgNodeUsrUpdate::XmsgImMgrOrgNodeUsrUpdate()
{

}

void XmsgImMgrOrgNodeUsrUpdate::handle(shared_ptr<XmsgNeUsr> nu, SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeUsrUpdateReq> req)
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
	SptrOrgUsr usr = XmsgImOrgMgr::instance()->findUsr(req->cgt());
	if (usr == nullptr)
	{
		trans->endDesc(RET_NOT_FOUND, "can not found user for channel global title: %s", req->cgt().c_str());
		return;
	}
	if (!req->has_infoaccountupsert() && req->infoaccountremove().empty())
	{
		if (req->name().empty() && !req->has_upsert() && req->remove().empty())
		{
			trans->endDesc(RET_FORBIDDEN, "update what?");
			return;
		}
		XmsgImOrgDb::instance()->future([trans, req, usr]
		{
			XmsgImMgrOrgNodeUsrUpdate::update2db(trans, req, usr);
		});
		return;
	}
	auto hlr = XmsgNeMgr::instance()->getHlr();
	if (hlr == nullptr)
	{
		LOG_ERROR("can not allocate x-msg-im-hlr, req: %s", req->ShortDebugString().c_str())
		trans->endDesc(RET_EXCEPTION, "system exception");
		return;
	}
	shared_ptr<XmsgImAuthOrgUpdateAccountInfoReq> r(new XmsgImAuthOrgUpdateAccountInfoReq());
	r->set_cgt(req->cgt());
	*(r->mutable_upsert()) = req->infoaccountupsert().kv();
	*(r->mutable_remove()) = req->infoaccountremove();
	XmsgImChannel::cast(hlr->channel)->begin(r, [trans, req, usr](SptrXiti itrans)
	{
		if (itrans->ret != RET_SUCCESS)
		{
			LOG_ERROR("update user account info failed, ret: %04X, desc: %s, req: %s", itrans->ret, itrans->desc.c_str(), req->ShortDebugString().c_str())
			trans->endDesc(itrans->ret, itrans->desc.c_str());
			return;
		}
		if (req->name().empty() && !req->has_upsert() && req->remove().empty()) 
		{
			shared_ptr<XmsgImMgrOrgNodeDisableOrEnableRsp> rsp(new XmsgImMgrOrgNodeDisableOrEnableRsp());
			XmsgMisc::insertKv(rsp->mutable_ext(), "ok", "true");
			trans->end(rsp);
			return;
		}
		LOG_DEBUG("update user account info successful, req: %s", req->ShortDebugString().c_str())
		XmsgImOrgDb::instance()->future([trans, req, usr] 
				{
					XmsgImMgrOrgNodeUsrUpdate::update2db(trans, req, usr);
				});
	}, nullptr, trans);
}

void XmsgImMgrOrgNodeUsrUpdate::update2db(SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeUsrUpdateReq> req, SptrOrgUsr usr)
{
	if (!req->name().empty())
		usr->name = req->name();
	auto it = req->infoaccountupsert().kv().find("usr");
	if (it != req->infoaccountupsert().kv().end() && it->second != usr->usr) 
		usr->usr = usr->usr;
	XmsgMisc::updateKv(req->upsert().kv(), req->remove(), *(usr->info->mutable_kv()));
	usr->ver = XmsgImOrgMgr::instance()->nextVer4usr(); 
	usr->uts = DateMisc::nowGmt0();
	if (!XmsgImOrgUsrCollOper::instance()->update(usr))
	{
		LOG_ERROR("update user info failed, may be database exception, user: %s", usr->toString().c_str())
		trans->endDesc(RET_EXCEPTION, "update user info failed, may be database exception");
		return;
	}
	LOG_INFO("update user info successful, usr: %s", usr->toString().c_str())
	XmsgImOrgMgr::instance()->updateOrgUsr(usr);
	XmsgImOrgMgr::instance()->setVer4usr(usr->ver);
	shared_ptr<XmsgImMgrOrgNodeDisableOrEnableRsp> rsp(new XmsgImMgrOrgNodeDisableOrEnableRsp());
	XmsgMisc::insertKv(rsp->mutable_ext(), "ok", "true");
	trans->end(rsp);
}

XmsgImMgrOrgNodeUsrUpdate::~XmsgImMgrOrgNodeUsrUpdate()
{

}

