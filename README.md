A C++ Client for KiteQ
========================
* More Details: https://github.com/blackbeans/kiteq

## 介绍
  这个库目前仅支持 zookeeper 为注册中心，暂不支持 etcd。

## 依赖
  依赖于 boost 库，我的开发环境为 boost1.53 
  依赖于 zookeeper 库， 链接：https://github.com/apache/zookeeper
  依赖于 C++ 的 zookeeper client 库， 链接 https://github.com/quguangjie/zkclient-cpp
  
## Producer 示例
···cpp
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
    string groupId = "pro-mts-test";
    string secretKey = "123456";
    string topic = "trade";
    string messageType = "pay-succ";
    long sendInterval = 1000;
    int clientNum = 1;
    int workerNum = 1;
    shared_ptr<Counter> UID(new Counter());

    shared_ptr<ClientConfigs> clientConfigs(new ClientConfigs(groupId, secretKey));
    shared_ptr<KiteClient> clients;
    {
        vector<string> topics;
        client = shared_ptr<KiteClient>(new DefaultKiteClient(zkAddr, clientConfigs));
        topics.push_back(topic);
        clients->setPublishTopics(topics);
        clients->start();
    }
    for(int i = 0; i < 10000; ++i)
    {
        int uid = UID->getAndIncrement();
        shared_ptr<StringMessage> message = buildMessage(topic, groupId, messageType, lexical_cast<string>(uid));
         cout << message->header().messageid() <<"  index:" <<i <<  endl;
        clients[0]->sendStringMessage(message);
    }
    return 0;
}
···


## Consumer 示例
···cpp
class testListenerAdapter : public ListenerAdapter
{
public:
    bool onMessage(shared_ptr<Message> message) {
        i ++;
        printf("testListenerAdapter::onMessage index %d id %s\n", i, message->getHeader().messageid().c_str());
        return true;
    }
    int  i = 0;
};
int main(int argc, char *argv[])
{
        string zkAddr = "172.16.1.96:2181";
        string groupId = "s-mts-test";
        string secretKey = "123456";
        string topic = "trade";
        string messageType = "pay-succ";
        int clientNum = 1;

        shared_ptr<ClientConfigs> clientConfigs(new ClientConfigs(groupId, secretKey));

        vector<shared_ptr<Binding> >bindings;
        shared_ptr<Binding> b (Binding::bindDirect(groupId, topic, messageType, 1000, true));
        bindings.push_back(b);
        shared_ptr<KiteClient> clients;

        shared_ptr<ListenerAdapter> l(new testListenerAdapter());
        
        {
            clients = shared_ptr<KiteClient>(new DefaultKiteClient(zkAddr, clientConfigs, l));
            clients->setBindings(bindings);
            clients->start();
        }
        
        while(1); slepp(1);
}

```

## 联系方式
   如有问题，请联系 QQ：583125219









