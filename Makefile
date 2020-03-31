#this is the makefile of the poker++ 
CXXFLAGS+= -Wall -O0 -g -std=c++11
GTKFLAGS = `/usr/bin/pkg-config gtkmm-3.0 --cflags --libs`

debug: CXXFLAGS += -g
debug: all

all: client

client: client.cpp mainwin.cpp *.h
	$(CXX) $(CXXFLAGS) client.cpp mainwin.cpp $(GTKFLAGS) -o client

clean:
	rm -f *.o *.gch client


