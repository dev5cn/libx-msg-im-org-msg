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

#include "XmsgImOrgNodeDeptUsrQuery.h"

XmsgImOrgNodeDeptUsrQuery::XmsgImOrgNodeDeptUsrQuery()
{

}

void XmsgImOrgNodeDeptUsrQuery::handle(shared_ptr<XmsgNeUsr> nu, SptrClient client, SptrXitp trans, shared_ptr<XmsgImOrgNodeDeptUsrQueryReq> req)
{
	if (req->dcgt().empty())
	{
		trans->endDesc(RET_FORMAT_ERROR, "department channel global title is null");
		return;
	}
	SptrCgt dcgt = ChannelGlobalTitle::parse(req->dcgt());
	if (dcgt == nullptr)
	{
		trans->endDesc(RET_FORMAT_ERROR, "department channel global title format error");
		return;
	}
	if (req->ucgt().empty())
	{
		trans->endDesc(RET_FORMAT_ERROR, "user channel global title is null");
		return;
	}
	SptrCgt ucgt = ChannelGlobalTitle::parse(req->ucgt());
	if (ucgt == nullptr)
	{
		trans->endDesc(RET_FORMAT_ERROR, "user channel global title format error");
		return;
	}
	SptrOrgDeptUsr deptUsr = XmsgImOrgMgr::instance()->findDeptUsr(req->dcgt(), req->ucgt());
	if (deptUsr == nullptr)
	{
		trans->endDesc(RET_NOT_FOUND, "can not found department and user relation");
		return;
	}
	shared_ptr<XmsgImOrgNodeDeptUsrQueryRsp> rsp(new XmsgImOrgNodeDeptUsrQueryRsp());
	rsp->set_name(deptUsr->name);
	rsp->set_enable(deptUsr->enable);
	rsp->mutable_info()->CopyFrom(*deptUsr->info);
	rsp->set_ver(deptUsr->ver);
	rsp->set_gts(deptUsr->gts);
	rsp->set_uts(deptUsr->uts);
	trans->end(rsp);
}

XmsgImOrgNodeDeptUsrQuery::~XmsgImOrgNodeDeptUsrQuery()
{

}
