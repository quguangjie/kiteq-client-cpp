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

#include "ClientConfigs.hpp"
#include "ListenerAdapter.hpp"
#include "Counter.hpp"
#include "DefaultKiteClient.h"
#include "BindingManager.h"
#include "ClientManager.h"
#include "ListenerAdapter.hpp"


class testListenerAdapter : public ListenerAdapter
{
public:
	bool onMessage(shared_ptr<Message> message) {
		printf("testListenerAdapter::onMessage\n");
		return true;
	}
};


int main(int argc, char *argv[])
{
        string zkAddr = "localhost:2181";
        string groupId = "s-mts-test";
        string secretKey = "123456";
        string topic = "trade";
        string messageType = "pay-succ";
        int clientNum = 1;

		shared_ptr<ClientConfigs> clientConfigs(new ClientConfigs(groupId, secretKey));

		vector<shared_ptr<Binding> >bindings;
		shared_ptr<Binding> b (Binding::bindDirect(groupId, topic, messageType, 1000, true));
		bindings.push_back(b);
		shared_ptr<KiteClient> clients[10];

		shared_ptr<ListenerAdapter> l(new testListenerAdapter());
		for (int i = 0; i < clientNum; i++)
		{
			clients[i] = shared_ptr<KiteClient>(new DefaultKiteClient(zkAddr, clientConfigs, l));
			clients[i]->setBindings(bindings);
			clients[i]->start();
		}
		while(1) sleep(1);

}


