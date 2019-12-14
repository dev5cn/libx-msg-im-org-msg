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

#include "XmsgImOrgSyncSub.h"

XmsgImOrgSyncSub::XmsgImOrgSyncSub()
{

}

void XmsgImOrgSyncSub::handle(shared_ptr<XmsgNeUsr> nu, SptrClient client, SptrXitp trans, shared_ptr<XmsgImOrgSyncSubReq> req)
{
	if (XmsgImOrgSubClientMgr::instance()->existed(client)) 
	{
		trans->endDesc(RET_DUPLICATE_OPER, "can not sub again");
		return;
	}
	auto rsp = XmsgImOrgMgr::instance()->snapShot(client); 
	trans->end(rsp);
	XmsgImOrgSyncSub::startPubOrgDeptEvent(client, req, rsp, req->ver4dept(), rsp->ver4deptlatest());
}

void XmsgImOrgSyncSub::startPubOrgDeptEvent(SptrClient client, shared_ptr<XmsgImOrgSyncSubReq> req, shared_ptr<XmsgImOrgSyncSubRsp> rsp, ullong start , ullong end )
{
	if (start >= end)
	{
		XmsgImOrgSyncSub::startPubOrgDeptUsrEvent(client, req, rsp, req->ver4deptusr(), rsp->ver4deptusrlatest());
		return;
	}
	list<shared_ptr<XmsgImOrgEvent>> lis;
	XmsgImOrgMgr::instance()->syncNextPageDept(start, end, lis);
	if (lis.empty()) 
	{
		XmsgImOrgSyncSub::startPubOrgDeptUsrEvent(client, req, rsp, req->ver4deptusr(), rsp->ver4deptusrlatest());
		return;
	}
	lis.sort([](auto a, auto b) 
	{
		return a->dept().ver() < b->dept().ver();
	});
	ullong ver = lis.back()->dept().ver(); 
	shared_ptr<XmsgImOrgSyncPubReq> pubReq(new XmsgImOrgSyncPubReq());
	for (auto& it : lis)
		pubReq->add_event()->CopyFrom(*it);
	client->begin(pubReq, [client, req, rsp, ver, end](ushort ret, const string& desc, shared_ptr<Message> r)
	{
		if(ret != RET_SUCCESS)
		{
			LOG_DEBUG("publish organization department increment event failed, we will close this connection, ret: %04X, desc: %s, client: %s", ret, desc.c_str(), client->toString().c_str())
			client->kick();
			return;
		}
		XmsgImOrgSyncSub::startPubOrgDeptEvent(client, req, rsp, ver, end); 
	}, client->oob);
}

void XmsgImOrgSyncSub::startPubOrgDeptUsrEvent(SptrClient client, shared_ptr<XmsgImOrgSyncSubReq> req, shared_ptr<XmsgImOrgSyncSubRsp> rsp, ullong start , ullong end )
{
	if (start >= end)
	{
		XmsgImOrgSyncSub::startPubOrgUsrEvent(client, req->ver4usr(), rsp->ver4usrlatest());
		return;
	}
	list<shared_ptr<XmsgImOrgEvent>> lis;
	XmsgImOrgMgr::instance()->syncNextPageDeptUsr(start, end, lis);
	if (lis.empty()) 
	{
		XmsgImOrgSyncSub::startPubOrgUsrEvent(client, req->ver4usr(), rsp->ver4usrlatest());
		return;
	}
	lis.sort([](auto a, auto b) 
	{
		return a->deptusr().ver() < b->deptusr().ver();
	});
	ullong ver = lis.back()->deptusr().ver(); 
	shared_ptr<XmsgImOrgSyncPubReq> pubReq(new XmsgImOrgSyncPubReq());
	for (auto& it : lis)
		pubReq->add_event()->CopyFrom(*it);
	client->begin(pubReq, [client, req, rsp, ver, end](ushort ret, const string& desc, shared_ptr<Message> r)
	{
		if(ret != RET_SUCCESS)
		{
			LOG_DEBUG("publish organization dept-usr increment event failed, we will close this connection, client: %s", client->toString().c_str())
			client->kick();
			return;
		}
		XmsgImOrgSyncSub::startPubOrgDeptUsrEvent(client, req, rsp, ver, end); 
	}, client->oob);
}

void XmsgImOrgSyncSub::startPubOrgUsrEvent(SptrClient client, ullong start , ullong end )
{
	if (start >= end)
	{
		LOG_DEBUG("have a client sync organization finished, client: %s", client->toString().c_str())
		client->finishedSyncPub();
		return;
	}
	list<shared_ptr<XmsgImOrgEvent>> lis;
	XmsgImOrgMgr::instance()->syncNextPageUsr(start, end, lis);
	if (lis.empty()) 
	{
		LOG_DEBUG("have a client sync organization finished, client: %s", client->toString().c_str())
		client->finishedSyncPub();
		return;
	}
	lis.sort([](auto a, auto b) 
	{
		return a->usr().ver() < b->usr().ver();
	});
	ullong ver = lis.back()->usr().ver(); 
	shared_ptr<XmsgImOrgSyncPubReq> pubReq(new XmsgImOrgSyncPubReq());
	for (auto& it : lis)
		pubReq->add_event()->CopyFrom(*it);
	client->begin(pubReq, [client, ver, end](ushort ret, const string& desc, shared_ptr<Message> r)
	{
		if(ret != RET_SUCCESS)
		{
			LOG_DEBUG("publish organization usr increment event failed, we will close this connection, client: %s", client->toString().c_str())
			client->kick();
			return;
		}
		XmsgImOrgSyncSub::startPubOrgUsrEvent(client, ver, end); 
	}, client->oob);
}

XmsgImOrgSyncSub::~XmsgImOrgSyncSub()
{

}

