CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -Wall -pthread -std=c++0x $(CXXOPTIMIZE)
src = main.cpp server.cpp connection.cpp config_parser.cc

all: webserver

webserver: $(src)
	$(CXX) -o $@ $(src) $(CXXFLAGS) -lboost_system

clean:
	rm -rf *.o webserver