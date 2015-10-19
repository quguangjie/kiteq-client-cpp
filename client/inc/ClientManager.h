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



#ifndef __CLIENTMANAGER_H
#define __CLIENTMANAGER_H

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>
#include  <boost/enable_shared_from_this.hpp>
#include "ConcurrentMap.hpp"
#include "NoKiteqServerException.h"

using namespace std;
using namespace boost;

class ZkClient;
class BindingManager;
class ClientConfigs;
class MessageListener;
class KiteIOClient;

class ClientManager: public boost::enable_shared_from_this<ClientManager>
{
public:
    ClientManager(shared_ptr<BindingManager> bindingManager, 
			      shared_ptr<ClientConfigs> clientConfigs, 
				  shared_ptr<MessageListener> listener);
    shared_ptr<KiteIOClient> get(const string &topic) throw (NoKiteqServerException);
    void refreshServers(const string &topic, list<string> newServerUris) ;
    shared_ptr<KiteIOClient> remove(const string& serverUrl);
    void     close() ;

private:
    shared_ptr<KiteIOClient> putIfAbsent(const string& serverUrl, shared_ptr<KiteIOClient> client);
    shared_ptr<KiteIOClient> createKiteIOClient(const string& serverUri) throw (std::runtime_error);
    void 	 createKiteQClient(const string &topic, const string& serverUri);
	void     kiteIOClientCheckThread();
	void     kiteIOClientReconnectThread(shared_ptr<KiteIOClient> c, const string &serverUri);
	
private:
    ConcurrentMap<string, shared_ptr<KiteIOClient> > waitingClients;
    ConcurrentMap<string, shared_ptr<KiteIOClient> > connMap;
    unordered_set<shared_ptr<KiteIOClient> >         clients;
	shared_ptr<mutex>                                setlock;

    shared_ptr<BindingManager>  bindingManager;
    shared_ptr<ClientConfigs>   clientConfigs;
    shared_ptr<MessageListener> listener;

	thread_group        gThread;
};


#endif 


