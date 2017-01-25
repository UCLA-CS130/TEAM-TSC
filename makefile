CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -Wall -pthread -std=c++0x $(CXXOPTIMIZE)
OBJ = build/main.o build/config_parser.o build/connection.o build/server.o

all: build/webserver

build/webserver: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) -lboost_system

build/main.o: src/main.cc src/server.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

build/config_parser.o: src/config_parser.cc src/config_parser.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

build/connection.o: src/connection.cc src/connection.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

build/server.o: src/server.cc src/server.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	rm -rf $(OBJ) build/webserver