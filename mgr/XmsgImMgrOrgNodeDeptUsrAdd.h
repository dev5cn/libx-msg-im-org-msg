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

#ifndef MGR_XMSGIMMGRORGNODEDEPTUSRADD_H_
#define MGR_XMSGIMMGRORGNODEDEPTUSRADD_H_

#include <libx-msg-im-org-core.h>

class XmsgImMgrOrgNodeDeptUsrAdd
{
public:
	static void handle(shared_ptr<XmsgNeUsr> nu, SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeDeptUsrAddReq> req); 
private:
	static void insert2db(SptrXitp trans, shared_ptr<XmsgImMgrOrgNodeDeptUsrAddReq> req, SptrCgt dcgt, SptrCgt ucgt); 
	XmsgImMgrOrgNodeDeptUsrAdd();
	virtual ~XmsgImMgrOrgNodeDeptUsrAdd();
};

#endif 
