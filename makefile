CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -Wall -pthread -std=c++0x $(CXXOPTIMIZE)
OBJ = build/main.o build/config_parser.o build/connection.o build/server.o build/config_handler.o 
GTEST_DIR=googletest/googletest
TEST_DIR=tests

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

build/config_handler.o: src/config_handler.cc  src/config_handler.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	rm -rf $(OBJ) build/webserver
build_tests: $(wildcard tests/**/*) $(wildcard src/**/*)
	$(CXX) -std=c++0x -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -pthread -c ${GTEST_DIR}/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o
	g++ -std=c++0x -isystem ${GTEST_DIR}/include -pthread tests/connection_test.cc src/connection.cc ${GTEST_DIR}/src/gtest_main.cc libgtest.a -lboost_system -o tests/connection_test

test:
	$(TEST_DIR)/connection_test
