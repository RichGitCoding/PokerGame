all: asio-1.12.2 src/Poker_client src/Poker_server

asio-1.12.2:
	tar xzf asio-1.12.2.tar.gz

GTKLINKFLAGS=$(shell pkg-config --libs gtkmm-3.0)
GTKCOMPILEFLAGS=$(shell pkg-config --cflags gtkmm-3.0)

CXXFLAGS+= -DASIO_STANDALONE -Wall -O0 -g -std=c++11
CPPFLAGS+=-I./include -I./asio-1.12.2/include -I./src ${GTKCOMPILEFLAGS}
LDLIBS+= -lpthread ${GTKLINKFLAGS}

clean:
	-rm -rf asio-1.12.2
	-rm -f src/Poker_client
	-rm -f src/Poker_server

