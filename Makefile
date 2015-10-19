include Makefile.in

OBJS = remoting/asioKiteIOClient.o remoting/ListenerManager.o client/ClientManager.o client/BindingManager.o client/DefaultKiteClient.o protocol/protobuf/generated/kite_remoting.pb.o  json/cJSON.o  protocol/protocol.o commons/ThreadPoolManager.o remoting/ResponseFuture.o protocol/KitePacket.o commons/KiteStats.o
PRODUCEROBJS := example/KiteqProducer.o
CONSUMEROBJS = example/KiteqConsumer.o
TXPRODUCEROBJS = example/KiteqTxProducer.o
CFLAGS  += -Wno-unknown-pragmas -finput-charset=UTF-8  -fexec-charset=UTF-8  -fwide-exec-charset=UTF-8 
CFLAGS  += -I client/inc  -I commons/ -Ijson -I protocol/  -I remoting/inc/ -I /root/figserver/trunk/appzkclient -I/root/figserver/trunk/zookeeper-3.4.6/src/c/include/ -I /root/figserver/trunk/zookeeper-3.4.6/src/c/generated/ 
LIBS += libkiteqclient.a /root/figserver/trunk/lib/libzkclient.a
LIBPATH += -lboost_thread-mt -lboost_filesystem-mt  -lboost_date_time -l zookeeper_mt -l protobuf -lpthread -lm


KiteqProducer = example/KiteqProducer
KiteqConsumer = example/KiteqConsumer
KiteqTxProducer = example/KiteqTxProducer

all: libkiteqclient.a

test : $(KiteqProducer) $(KiteqConsumer) $(KiteqTxProducer)

libkiteqclient.a : $(OBJS)
	$(AR) cr $@ $^

$(KiteqProducer):$(PRODUCEROBJS) libkiteqclient.a
	$(CPLUSPLUS) $(CFLAGS) $(INCLUDE) $(PRODUCEROBJS) $(LIBS) -o $@ $(LIBPATH)
$(KiteqConsumer):$(CONSUMEROBJS) libkiteqclient.a
	$(CPLUSPLUS) $(CFLAGS) $(INCLUDE) $(CONSUMEROBJS) $(LIBS) -o $@ $(LIBPATH)
$(KiteqTxProducer):$(TXPRODUCEROBJS) libkiteqclient.a
	$(CPLUSPLUS) $(CFLAGS) $(INCLUDE) $(TXPRODUCEROBJS) $(LIBS) -o $@ $(LIBPATH)

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE) $< -c -o $@ 

%.o:%.cpp
	$(CPLUSPLUS) $(CFLAGS) $(INCLUDE) $< -c -o $@ 

.PHONY: clean
	
clean:
	find . -name "*.o" | xargs rm *.o -rf 
	rm *.a -rf 




