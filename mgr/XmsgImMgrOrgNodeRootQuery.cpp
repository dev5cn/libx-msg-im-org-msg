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

#include "XmsgImMgrOrgNodeRootQuery.h"

XmsgImMgrOrgNodeRootQuery::XmsgImMgrOrgNodeRootQuery()
{

}

void XmsgImMgrOrgNodeRootQuery::handle(shared_ptr<XmsgNeUsr> nu, SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeRootQueryReq> req)
{
	shared_ptr<XmsgImMgrOrgNodeRootQueryRsp> rsp(new XmsgImMgrOrgNodeRootQueryRsp());
	SptrOrgDept root = XmsgImOrgMgr::instance()->getRoot();
	rsp->set_cgt(root->cgt->toString());
	rsp->set_name(root->name);
	rsp->mutable_info()->CopyFrom(*root->info);
	rsp->set_gts(root->gts);
	rsp->set_uts(root->uts);
	trans->end(rsp);
}

XmsgImMgrOrgNodeRootQuery::~XmsgImMgrOrgNodeRootQuery()
{

}

