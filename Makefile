include Makefile.in

OBJS = remoting/asioKiteIOClient.o remoting/ListenerManager.o  remoting/ResponseFuture.o \
       client/manager/ClientManager.o client/manager/BindingManager.o client/manager/DefaultKiteClient.o    \
	   client/binding/Binding.o client/message/MessageUtils.o  \
       protocol/protobuf/generated/kite_remoting.pb.o  protocol/protocol.o protocol/KitePacket.o protocol/KitePacketHeader.o \
       commons/ThreadPoolManager.o commons/KiteStats.o    json/cJSON.o

PRODUCEROBJS := example/KiteqProducer.o
CONSUMEROBJS = example/KiteqConsumer.o
TXPRODUCEROBJS = example/KiteqTxProducer.o
CFLAGS  += -Wno-unknown-pragmas -finput-charset=UTF-8  -fexec-charset=UTF-8  -fwide-exec-charset=UTF-8 
CFLAGS  += -I client/manager/  -I client/message/ -Iclient/util -Iclient/binding -I commons/ -Ijson -I protocol/  -I remoting/inc/ 
CFLAGS  += -I /root/figserver/trunk/appzkclient -I/root/figserver/trunk/zookeeper-3.4.6/src/c/include/ -I /root/figserver/trunk/zookeeper-3.4.6/src/c/generated/ 
LIBS += libkiteqclient.a ../zkclient-cpp//libzkclient.a
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




