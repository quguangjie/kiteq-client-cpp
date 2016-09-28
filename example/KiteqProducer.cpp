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

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/algorithm/string.hpp>

#include "ClientConfigs.hpp"
#include "ListenerAdapter.hpp"
#include "Counter.hpp"
#include "DefaultKiteClient.h"  
#include "BindingManager.h"  
#include "ClientManager.h"  

using namespace std;
using namespace boost;

static shared_ptr<StringMessage> buildMessage(const string &topic, const string &groupId, const string &messageType, const string &body) 
{
	long  now = time(NULL);
	uuids::random_generator  rgen;
	uuids::uuid u = rgen();
	Header * _header = new Header();
	//cout <<  replace_all_copy(to_string(u), "-", "") << endl; 
	_header->set_messageid(replace_all_copy(to_string(u), "-", ""));
	//_header->set_messageid("0xc208a6f810");
	_header->set_topic("trade");
	_header->set_messagetype("pay-succ");
	_header->set_expiredtime(1845173857);
	_header->set_deliverlimit(100);
	_header->set_groupid("go-kite-test");
	_header->set_commit(true);
	_header->set_fly(false);

	shared_ptr<StringMessage> m(new StringMessage());
	m->set_body("hello world");
	m->set_allocated_header(_header);
	return m;
}

int main(int argc, char *argv[])
{
	string zkAddr = "172.16.1.96:2181";
	string groupId = "s-mts-test";
	string secretKey = "123456";
    string topic = "trade";
    string messageType = "pay-succ";
    long sendInterval = 1000;
    int clientNum = 1;
    int workerNum = 1;
	shared_ptr<Counter> UID(new Counter());

	shared_ptr<ClientConfigs> clientConfigs(new ClientConfigs(groupId, secretKey));
	cout << clientConfigs->groupId << endl;
	cout << clientConfigs->secretKey << endl;

	shared_ptr<KiteClient> clients[10];
	for (int i = 0; i < clientNum; i++) 
	{
		vector<string> topics;
		clients[i] = shared_ptr<KiteClient>(new DefaultKiteClient(zkAddr, clientConfigs));
		topics.push_back(topic);
		clients[i]->setPublishTopics(topics);
		clients[i]->start();
	}
	sleep(3);
	for(int i = 0; i < 10000; ++i)
	{
		int uid = UID->getAndIncrement();
		shared_ptr<StringMessage> message = buildMessage(topic, groupId, messageType, lexical_cast<string>(uid));
	cout << message->header().messageid() <<"  index:" <<i <<  endl;
		clients[0]->sendStringMessage(message);
	}
	while(1)sleep(1);


	return 0; 
}
