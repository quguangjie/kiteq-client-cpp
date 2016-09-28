/*
 *  GuangJie Qu <qgjie456@163.com>
 *  Copyright (C) 2015  BeiJing PZYJ Technology Co. Ltd.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, 
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "asioKiteIOClient.h"
#include "KiteResponse.hpp"
#include "ResponseFuture.h"

std::map<int, shared_ptr<ResponseFuture> > ResponseFuture::futureMap;
boost::mutex   ResponseFuture::maplock;

ResponseFuture::ResponseFuture(int id): requestId(id) 
{
}
void  ResponseFuture::ResponseFutureUsed()
{
	boost::mutex::scoped_lock lock(maplock);

	map<int, shared_ptr<ResponseFuture> >::iterator  it = futureMap.find(requestId);
	if(it == futureMap.end())
		futureMap.insert(make_pair(requestId, shared_from_this()));	
}

void ResponseFuture::receiveResponse(shared_ptr<KiteResponse> resp) 
{
	boost::mutex::scoped_lock lock(maplock);

    int requestId = resp->getRequestId();
	map<int, shared_ptr<ResponseFuture> >::iterator  it = futureMap.find(requestId);
	if(it != futureMap.end())
	{
    	shared_ptr<ResponseFuture> f = it->second;
		futureMap.erase(requestId);
        f->setResponse(resp);
	}
}

void ResponseFuture::setResponse(shared_ptr<KiteResponse> resp) 
{
	set_value(resp);
}

shared_ptr<KiteResponse> ResponseFuture::getResponse()
{
	unique_future<shared_ptr<KiteResponse> > f = get_future();
	f.wait();
	return f.get();
}

shared_ptr<KiteResponse> ResponseFuture::getResponse(long timeout) 
{
	unique_future<shared_ptr<KiteResponse> > f = get_future();
	f.timed_wait((boost::posix_time::milliseconds)(timeout));
	if(f.has_value())
		return f.get();
	maplock.lock();
	futureMap.erase(requestId);
	maplock.unlock();
	return shared_ptr<KiteResponse>();
}

int  ResponseFuture::getRequestId()
{
	return requestId;
}


