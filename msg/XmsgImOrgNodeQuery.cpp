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

#include "XmsgImOrgNodeQuery.h"

XmsgImOrgNodeQuery::XmsgImOrgNodeQuery()
{

}

void XmsgImOrgNodeQuery::handle(shared_ptr<XmsgNeUsr> nu, SptrClient client, SptrXitp trans, shared_ptr<XmsgImOrgNodeQueryReq> req)
{
	if (req->cgt().empty())
	{
		trans->endDesc(RET_FORMAT_ERROR, "channel global title is null");
		return;
	}
	SptrCgt cgt = ChannelGlobalTitle::parse(req->cgt());
	if (cgt == nullptr)
	{
		trans->endDesc(RET_FORMAT_ERROR, "channel global title format error");
		return;
	}
	if (cgt->isUsr())
	{
		SptrOrgUsr usr = XmsgImOrgMgr::instance()->findUsr(req->cgt());
		if (usr == nullptr)
		{
			trans->end(RET_NOT_FOUND);
			return;
		}
		shared_ptr<XmsgImOrgNodeQueryRsp> rsp(new XmsgImOrgNodeQueryRsp());
		rsp->set_name(usr->name);
		rsp->set_enable(usr->enable);
		rsp->set_type(XmsgImOrgNodeType::X_MSG_IM_ORG_NODE_TYPE_LEAF);
		rsp->mutable_info()->CopyFrom(*usr->info);
		rsp->set_ver(usr->ver);
		rsp->set_gts(usr->gts);
		rsp->set_uts(usr->uts);
		trans->end(rsp);
		return;
	}
	SptrOrgDept dept = XmsgImOrgMgr::instance()->findDept(req->cgt());
	if (dept == nullptr)
	{
		trans->end(RET_NOT_FOUND);
		return;
	}
	shared_ptr<XmsgImOrgNodeQueryRsp> rsp(new XmsgImOrgNodeQueryRsp());
	if (dept->pcgt != nullptr)
		rsp->set_pgt(dept->pcgt->toString());
	rsp->set_name(dept->name);
	rsp->set_enable(dept->enable);
	rsp->set_type(XmsgImOrgNodeType::X_MSG_IM_ORG_NODE_TYPE_BRANCH);
	rsp->mutable_info()->CopyFrom(*dept->info);
	rsp->set_ver(dept->ver);
	rsp->set_gts(dept->gts);
	rsp->set_uts(dept->uts);
	trans->end(rsp);
}

XmsgImOrgNodeQuery::~XmsgImOrgNodeQuery()
{

}

