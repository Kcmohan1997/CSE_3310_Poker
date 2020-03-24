#this is the makefile of the poker++ 

CXXFLAGS += -Wall -g -O0 -std=c++11

all: poker

poker: main.o
	$(CXX) $(CXXFLAGS) main.o -o poker
main.o: main.cpp *.h
	$(CXX) $(CXXFLAGS) -c main.cpp

clean:
	rm -rf *o poker


