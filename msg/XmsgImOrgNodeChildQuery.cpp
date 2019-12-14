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

#include "XmsgImOrgNodeChildQuery.h"

XmsgImOrgNodeChildQuery::XmsgImOrgNodeChildQuery()
{

}

void XmsgImOrgNodeChildQuery::handle(shared_ptr<XmsgNeUsr> nu, SptrClient client, SptrXitp trans, shared_ptr<XmsgImOrgNodeChildQueryReq> req)
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
	list<shared_ptr<XmsgImOrgNodeChild>> child;
	XmsgImOrgMgr::instance()->findChild(req->cgt(), child);
	if (child.empty())
	{
		trans->end(RET_NO_RECORD);
		return;
	}
	shared_ptr<XmsgImOrgNodeChildQueryRsp> rsp(new XmsgImOrgNodeChildQueryRsp());
	for (auto& it : child)
		rsp->add_child()->CopyFrom(*it);
	trans->end(rsp);
}

XmsgImOrgNodeChildQuery::~XmsgImOrgNodeChildQuery()
{

}

