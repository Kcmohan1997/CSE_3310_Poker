#this is the makefile of the poker++ 
CXXFLAGS += --std=c++11
GTKFLAGS = `/usr/bin/pkg-config gtkmm-3.0 --cflags --libs`

debug: CXXFLAGS += -g
debug: all

all: mainwin 

mainwin: main.cpp mainwin.cpp *.h
	$(CXX) $(CXXFLAGS) main.cpp mainwin.cpp $(GTKFLAGS) -o mainwin
clean:
	rm -f *.o *.gch mainwin


