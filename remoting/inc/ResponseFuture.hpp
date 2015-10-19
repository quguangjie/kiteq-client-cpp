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

#ifndef   __RESPONSEFUTURE_HPP
#define   __RESPONSEFUTURE_HPP

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/future.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace std; 
using namespace boost; 

class BindingManager;
class ResponseFuture: public promise<shared_ptr<KiteResponse> >, public enable_shared_from_this<ResponseFuture>
{
public:
    ResponseFuture(int id): requestId(id) 
	{
    }
	void  ResponseFutureUsed()
	{
		boost::mutex::scoped_lock lock(maplock);

		map<int, shared_ptr<ResponseFuture> >::iterator  it = futureMap.find(requestId);
		if(it == futureMap.end())
			futureMap.insert(make_pair(requestId, shared_from_this()));	
	}
    
    static void receiveResponse(shared_ptr<KiteResponse> resp) 
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
    
    void setResponse(shared_ptr<KiteResponse> resp) 
	{
		set_value(resp);
    }

    shared_ptr<KiteResponse> getResponse()
	{
		unique_future<shared_ptr<KiteResponse> > f = get_future();
		f.wait();
		return f.get();
    }

    shared_ptr<KiteResponse> getResponse(long timeout) 
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

	int  getRequestId()
	{
		return requestId;
	}
private:
    static 	map<int, shared_ptr<ResponseFuture> > futureMap; 
	static 	mutex maplock;
    int 	requestId;
}; 



#endif 



