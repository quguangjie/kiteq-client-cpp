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
#include "TxCallback.h"

class testListenerAdapter : public ListenerAdapter
{
public:
	bool onMessage(shared_ptr<Message> message) {
		printf("testListenerAdapter::onMessage\n");
		return true;
	}
	bool onMessageCheck(shared_ptr<TxResponse> response) {
		printf("testListenerAdapter::onMessageCheck\n");
		return false;
	}
};

class testTxCallback : public TxCallback
{
public:
    void doTransaction(shared_ptr<TxResponse> tx) throw (std::runtime_error)
	{
		printf("testTxResponse::doTransaction\n");
		tx->commit();
	}
};

static shared_ptr<StringMessage> buildMessage(const string &topic, const string &groupId, const string &messageType, const string &body)
{
	long  now = time(NULL);
	uuids::random_generator  rgen;
	uuids::uuid u = rgen();
	Header * _header = new Header();
	cout <<  replace_all_copy(to_string(u), "-", "") << endl;
	_header->set_messageid(replace_all_copy(to_string(u), "-", ""));
	_header->set_topic("trade");
	_header->set_messagetype("pay-succ");
	_header->set_expiredtime(1445173857);
	_header->set_deliverlimit(100);
	_header->set_groupid("go-kite-test");
	_header->set_commit(false);
	_header->set_fly(false);

	shared_ptr<StringMessage> m(new StringMessage());
	m->set_body("hello world");
	m->set_allocated_header(_header);
	return m;
}


int main(int argc, char *argv[])
{
	string zkAddr = "127.0.0.1:2181";
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
	shared_ptr<ListenerAdapter> l(new testListenerAdapter());
	for (int i = 0; i < clientNum; i++)
	{
		vector<string> topics;
		clients[i] = shared_ptr<KiteClient>(new DefaultKiteClient(zkAddr, clientConfigs, l));
		topics.push_back(topic);
		clients[i]->setPublishTopics(topics);
		clients[i]->start();
	}
	sleep(3);
 	shared_ptr<testTxCallback> tr(new testTxCallback());
	while(1)
	{
		int uid = UID->getAndIncrement();
		shared_ptr<StringMessage> message = buildMessage(topic, groupId, messageType,                  lexical_cast<string>(uid));
		clients[0]->sendTxMessage(message, tr);
	}
	while(1)sleep(1);
}


