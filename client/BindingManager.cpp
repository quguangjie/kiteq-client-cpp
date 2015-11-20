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

#include <boost/date_time/gregorian/gregorian.hpp>  
#include <boost/date_time/posix_time/posix_time.hpp>  
#include <stdexcept>

#include "BindingManager.h"
#include "ZkClient.h"
#include "ClientManager.h"
#include "Binding.hpp"

class watchReqNodeChild :public IZkChildListener
{
public:
	watchReqNodeChild(shared_ptr<ZkClient> cli, shared_ptr<BindingManager> pManager, const string &parentPath, const string &topic):IZkChildListener(parentPath), _topic(topic)
	{
		_zkcli = cli;
		_bindingManager =pManager;

	}
	~watchReqNodeChild(){}
	void handleChildChange(const string &parentPath, list<string> &currentChildren)
	{
		_bindingManager->refreshTopicServers(_topic, currentChildren);
	}
	void handleParentChange(const string &parentPath){}
private:
	string                     _topic;
	shared_ptr<ZkClient>       _zkcli;
	shared_ptr<BindingManager> _bindingManager;
};

BindingManager::BindingManager(const string &zkAddr)
{
	zkc = shared_ptr<ZkClient>(new ZkClient(zkAddr, ZKBASE_SESSIONTIMEOUT, ZKBASE_CONNECTIONTIMEOUT));
}
shared_ptr<BindingManager> BindingManager::getInstance(const string &zkAddr)
{
	boost::mutex::scoped_lock lock(ismutex);
    map<string, shared_ptr<BindingManager> >::iterator it = instances.find(zkAddr);
	if(it != instances.end())
	{
		return it->second;
	}
	try {
		shared_ptr<BindingManager> p(new BindingManager(zkAddr));
		if(p == NULL)
			return p;
		instances.insert(make_pair(zkAddr, p));
		return p;
	} catch (ZkExceptionConnectionLoss e) {
	} catch (std::exception e) {
	}
	return shared_ptr<BindingManager>();
}
list<string> BindingManager::getServerList(const string &topic)
{
	boost::mutex::scoped_lock lock(tsmutex);
    map<string, list<string> >::iterator it = topicServerMap.find(topic);
	if(it == topicServerMap.end())
	{
		string path(SERVER_PATH);
		path.append(topic);
		shared_ptr<watchReqNodeChild> childListener (new watchReqNodeChild(zkc, shared_from_this(), path, topic));
		try{ 
			printf("BindingManager::getServerList %s\n",path.c_str());
			list<string> reqSerlist = zkc->getChildren(path);
			zkc->subscribeChildChanges(path, childListener);
			topicServerMap.insert(make_pair(topic, reqSerlist));
			//printf("BindingManager::getServerList serlist size %ld\n",reqSerlist.size());
			return reqSerlist; 
		}catch (ZkExceptionNoNode &e){
			cout << e.what() << endl;
		}catch (ZkException &e){
			cout << e.what() << endl;
		}	
		return list<string>();
	}
	return it->second;
}
void BindingManager::refreshTopicServers(const string &topic, const list<string> &servers)
{
	boost::mutex::scoped_lock lock(tsmutex);
	shared_ptr<ClientManager>  clientManager;
	if(clientManagerMap.get(topic, clientManager))
	{
		clientManager->refreshServers(topic, servers);
		topicServerMap.insert(make_pair(topic, servers));
	}
}

void BindingManager::registerClientManager(const string &topic, shared_ptr<ClientManager> clientManager)
{
	shared_ptr<ClientManager> _clientManager;
	clientManagerMap.putIfAbsent(topic, clientManager, _clientManager);
}
void BindingManager::registerProducer(const string &topic, const string &groupId, const string &producerName)
{
	string groupPath = PRODUCER_ZK_PATH + "/" + topic + "/" + groupId;
	boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();  
	boost::posix_time::time_facet* tf = new boost::posix_time::time_facet("%Y%m%d%H%M%S.%F");  
	std::stringstream ss;  
	ss.imbue(std::locale(std::cout.getloc(), tf));  
	ss << pt;  
	string path = groupPath  + "/" + producerName + "@";
	path.append(ss.str());
	//printf("BindingManager::registerProducer: path %s\n", path.c_str());
	while(true)
	{
		try{
			try {
				zkc->deleteRecursive(path);
			} catch (...) {}
			zkc->createPersistent(groupPath, true);
			zkc->createEphemeral(path);
			printf("[%s.%d]\n", __FUNCTION__,__LINE__);
			break;
		}catch (ZkException &e){
		}
	}
}
void BindingManager::registerConsumer(const vector<shared_ptr<Binding> >& bindings)
{
	vector<shared_ptr<Binding > >::const_iterator it;
	map<pair<string, string>, list<shared_ptr<Binding> > >::iterator bit;
	map<pair<string, string>, list<shared_ptr<Binding> > > bindingsMap;

	for (it = bindings.begin(); it != bindings.end(); ++it) 
	{

		pair<string, string> bindingPair = make_pair((*it)->getTopic(), (*it)->getGroupId());
		bit = bindingsMap.find(bindingPair);
		if(bit == bindingsMap.end())
		{
			list<shared_ptr<Binding> >_bindings;
			_bindings.push_back(*it);
			bindingsMap.insert(make_pair(bindingPair, _bindings));
		}
		else
		{
			bit->second.push_back(*it);
		}
	}
	for (bit = bindingsMap.begin(); bit != bindingsMap.end(); ++bit)
	{
		string path = CONSUMER_ZK_PATH + "/" + bit->first.first + "/" + bit->first.second + "-bind";
		bool nodeExisted = zkc->exists(path);
		if(!nodeExisted)
			zkc->createPersistent(path, true);
		string  data;
		Binding::toJson(bit->second, data);
		zkc->writeData(path, data.c_str());
	}
}
void BindingManager::close()
{
	if(zkc != NULL)
		zkc->close();
}
void BindingManager::setTopicServerList(const list<string> &currentChildren)
{
}
shared_ptr<ZkClient> BindingManager::getCurator()
{
	return zkc;
}

const string BindingManager::SERVER_PATH = "/kiteq/server/";
const string BindingManager::CONSUMER_ZK_PATH = "/kiteq/sub";
const string BindingManager::PRODUCER_ZK_PATH = "/kiteq/pub";
map<string, shared_ptr<BindingManager> > BindingManager::instances;
boost::mutex                    BindingManager::ismutex;



