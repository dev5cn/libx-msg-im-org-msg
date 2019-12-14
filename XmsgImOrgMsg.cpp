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

#include "XmsgImOrgMsg.h"
#include "mgr/XmsgImMgrNeNetLoad.h"
#include "mgr/XmsgImMgrNeXscServerQuery.h"
#include "mgr/XmsgImMgrNeXscWorkerCount.h"
#include "mgr/XmsgImMgrOrgNodeDeptAdd.h"
#include "mgr/XmsgImMgrOrgNodeDeptUpdate.h"
#include "mgr/XmsgImMgrOrgNodeDeptUsrAdd.h"
#include "mgr/XmsgImMgrOrgNodeDeptUsrDisableOrEnable.h"
#include "mgr/XmsgImMgrOrgNodeDeptUsrUpdate.h"
#include "mgr/XmsgImMgrOrgNodeDisableOrEnable.h"
#include "mgr/XmsgImMgrOrgNodeRootQuery.h"
#include "mgr/XmsgImMgrOrgNodeUsrAdd.h"
#include "mgr/XmsgImMgrOrgNodeUsrUpdate.h"
#include "msg/XmsgImOrgNodeChildQuery.h"
#include "msg/XmsgImOrgNodeDeptUsrQuery.h"
#include "msg/XmsgImOrgNodeQuery.h"
#include "msg/XmsgImOrgSyncSub.h"
#include "ne/XmsgApClientLost.h"
#include "ne/XmsgNeAuth.h"

XmsgImOrgMsg::XmsgImOrgMsg()
{

}

void XmsgImOrgMsg::init(shared_ptr<XmsgImN2HMsgMgr> priMsgMgr)
{
	X_MSG_N2H_PRPC_BEFOR_AUTH(priMsgMgr, XmsgNeAuthReq, XmsgNeAuthRsp, XmsgNeAuth::handle)
	X_MSG_N2H_PRPC_AFTER_AUTH(priMsgMgr, XmsgImMgrNeNetLoadReq, XmsgImMgrNeNetLoadRsp, XmsgImMgrNeNetLoad::handle)
	X_MSG_N2H_PRPC_AFTER_AUTH(priMsgMgr, XmsgImMgrNeXscServerQueryReq, XmsgImMgrNeXscServerQueryRsp, XmsgImMgrNeXscServerQuery::handle)
	X_MSG_N2H_PRPC_AFTER_AUTH(priMsgMgr, XmsgImMgrNeXscWorkerCountReq, XmsgImMgrNeXscWorkerCountRsp, XmsgImMgrNeXscWorkerCount::handle)
	X_MSG_N2H_PRPC_AFTER_AUTH(priMsgMgr, XmsgImMgrOrgNodeDeptAddReq, XmsgImMgrOrgNodeDeptAddRsp, XmsgImMgrOrgNodeDeptAdd::handle)
	X_MSG_N2H_PRPC_AFTER_AUTH(priMsgMgr, XmsgImMgrOrgNodeDeptUsrAddReq, XmsgImMgrOrgNodeDeptUsrAddRsp, XmsgImMgrOrgNodeDeptUsrAdd::handle)
	X_MSG_N2H_PRPC_AFTER_AUTH(priMsgMgr, XmsgImMgrOrgNodeDeptUsrDisableOrEnableReq, XmsgImMgrOrgNodeDeptUsrDisableOrEnableRsp, XmsgImMgrOrgNodeDeptUsrDisableOrEnable::handle)
	X_MSG_N2H_PRPC_AFTER_AUTH(priMsgMgr, XmsgImMgrOrgNodeDeptUsrUpdateReq, XmsgImMgrOrgNodeDeptUsrUpdateRsp, XmsgImMgrOrgNodeDeptUsrUpdate::handle)
	X_MSG_N2H_PRPC_AFTER_AUTH(priMsgMgr, XmsgImMgrOrgNodeDisableOrEnableReq, XmsgImMgrOrgNodeDisableOrEnableRsp, XmsgImMgrOrgNodeDisableOrEnable::handle)
	X_MSG_N2H_PRPC_AFTER_AUTH(priMsgMgr, XmsgImMgrOrgNodeDeptUpdateReq, XmsgImMgrOrgNodeDeptUpdateRsp, XmsgImMgrOrgNodeDeptUpdate::handle)
	X_MSG_N2H_PRPC_AFTER_AUTH(priMsgMgr, XmsgImMgrOrgNodeRootQueryReq, XmsgImMgrOrgNodeRootQueryRsp, XmsgImMgrOrgNodeRootQuery::handle)
	X_MSG_N2H_PRPC_AFTER_AUTH(priMsgMgr, XmsgImMgrOrgNodeUsrAddReq, XmsgImMgrOrgNodeUsrAddRsp, XmsgImMgrOrgNodeUsrAdd::handle)
	X_MSG_N2H_PRPC_AFTER_AUTH(priMsgMgr, XmsgImMgrOrgNodeUsrUpdateReq, XmsgImMgrOrgNodeUsrUpdateRsp, XmsgImMgrOrgNodeUsrUpdate::handle)
	X_MSG_H2N_PRPC_AFTER_AUTH(XmsgAp, XmsgImOrgNodeChildQueryReq, XmsgImOrgNodeChildQueryRsp, XmsgImOrgNodeChildQuery::handle)
	X_MSG_H2N_PRPC_AFTER_AUTH(XmsgAp, XmsgImOrgNodeDeptUsrQueryReq, XmsgImOrgNodeDeptUsrQueryRsp, XmsgImOrgNodeDeptUsrQuery::handle)
	X_MSG_H2N_PRPC_AFTER_AUTH(XmsgAp, XmsgImOrgNodeQueryReq, XmsgImOrgNodeQueryRsp, XmsgImOrgNodeQuery::handle)
	X_MSG_H2N_PRPC_AFTER_AUTH(XmsgAp, XmsgImOrgSyncSubReq, XmsgImOrgSyncSubRsp, XmsgImOrgSyncSub::handle)
	X_MSG_H2N_PRPC_AFTER_AUTH_UNI(XmsgImHlr, XmsgApClientLostNotice, XmsgApClientLost::handle)
}

XmsgImOrgMsg::~XmsgImOrgMsg()
{

}

