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


#ifndef  __BINDINGMANAGER_H
#define  __BINDINGMANAGER_H

#include <string>
#include <list>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include  <boost/enable_shared_from_this.hpp>
#include "Binding.hpp"
#include "ConcurrentMap.hpp"

using namespace std;
using namespace boost;

#define     ZKBASE_SESSIONTIMEOUT        (30000)   
#define     ZKBASE_CONNECTIONTIMEOUT      (30)  

class ZkClient;
class ClientManager;
class BindingManager : public boost::enable_shared_from_this<BindingManager>
{
public: 
    static const string SERVER_PATH;
    
    static shared_ptr<BindingManager> getInstance(const string &zkAddr);
    list<string> getServerList(const string &topic);
    void registerClientManager(const string &topic, shared_ptr<ClientManager> clientManager);
    void registerProducer(const string &topic, const string &groupId, const string &producerName);
    void registerConsumer(const vector<shared_ptr<Binding> >& bindings);
    void close();
	void setTopicServerList(const list<string> &currentChildren);
	void refreshTopicServers(const string &topic, const list<string> &servers);
    shared_ptr<ZkClient> getCurator();

private:
    BindingManager(const string &zkAddr);

private:
    static const string PRODUCER_ZK_PATH ;
    static const string CONSUMER_ZK_PATH ;
    static map<string, shared_ptr<BindingManager> > instances;
	static boost::mutex                    ismutex;
    map<string, list<string> > 			   topicServerMap;
	boost::mutex                           tsmutex;
    ConcurrentMap<string, shared_ptr<ClientManager> > clientManagerMap;
	shared_ptr<ZkClient> zkc;
};

#endif 



