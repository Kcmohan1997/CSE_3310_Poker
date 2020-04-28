# makefile of poker++


all: asio-1.12.2 src/chat_server src/client src/json 

asio-1.12.2:
	tar xzf asio-1.12.2.tar.gz

GTKLINKFLAGS=$(shell pkg-config --libs gtkmm-3.0)
GTKCOMPILEFLAGS=$(shell pkg-config --cflags gtkmm-3.0)

CXXFLAGS+= -DASIO_STANDALONE -Wall -O0 -g -std=c++11
CPPFLAGS+= -I./include/ -I./asio-1.12.2/include -I./src 
LDLIBS += -lpthread 

src/client: src/client_window.cpp src/chat_client.cpp
	${CXX} ${CXXFLAGS} ${CPPFLAGS} ${GTKCOMPILEFLAGS}  -o $@ $^  ${LDLIBS} ${GTKLINKFLAGS}

src/chat_server: src/dealer.cpp src/chat_server.cpp
	${CXX} ${CXXFLAGS} ${CPPFLAGS} ${GTKCOMPILEFLAGS}  -o $@ $^  ${LDLIBS} ${GTKLINKFLAGS}

clean:
	-rm -rf asio-1.12.2
	-rm -f src/client
	-rm -f src/chat_server	
	-rm -f src/json
	-rm -f src/dealer
