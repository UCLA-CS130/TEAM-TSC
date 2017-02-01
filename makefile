CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -Wall -pthread -std=c++0x $(CXXOPTIMIZE)
OBJ = build/main.o build/config_parser.o build/connection.o build/server.o build/config_handler.o 
GTEST_DIR=googletest/googletest
GMOCK_DIR=googletest/googlemock
TEST_DIR=tests
SRC_DIR=src
BUILD_DIR=build

all: build/webserver

$(BUILD_DIR)/webserver: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) -lboost_system

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cc $(SRC_DIR)/server.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

$(BUILD_DIR)/config_parser.o: $(SRC_DIR)/config_parser.cc $(SRC_DIR)/config_parser.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

$(BUILD_DIR)/connection.o: $(SRC_DIR)/connection.cc $(SRC_DIR)/connection.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

$(BUILD_DIR)/server.o: $(SRC_DIR)/server.cc $(SRC_DIR)/server.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

$(BUILD_DIR)/config_handler.o: $(SRC_DIR)/config_handler.cc  $(SRC_DIR)/config_handler.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

.PHONY: clean test

clean:
	rm -rf $(OBJ) $(BUILD_DIR)/webserver *.a *.o $(TEST_DIR)/connection_test $(TEST_DIR)/server_test $(TEST_DIR)/config_parser_test

test_build: ${GTEST_DIR}/src/gtest-all.cc ${GMOCK_DIR}/src/gmock-all.cc
	g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    	-pthread -c ${GTEST_DIR}/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o

	g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    	-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    	-pthread -c ${GMOCK_DIR}/src/gmock-all.cc
	ar -rv libgmock.a gtest-all.o gmock-all.o


test: test_build $(TEST_DIR)/config_parser_test $(TEST_DIR)/connection_test $(TEST_DIR)/server_test $(TEST_DIR)/config_handler_test 

$(TEST_DIR)/connection_test: $(TEST_DIR)/connection_test.cc
	g++ -std=c++0x -isystem ${GTEST_DIR}/include -pthread $(TEST_DIR)/connection_test.cc $(SRC_DIR)/connection.cc ${GTEST_DIR}/src/gtest_main.cc libgtest.a -lboost_system -o $(TEST_DIR)/connection_test

$(TEST_DIR)/config_parser_test: $(SRC_DIR)/config_parser.cc $(TEST_DIR)/config_parser_test.cc
	g++ -std=c++0x -isystem ${GTEST_DIR}/include -pthread $(TEST_DIR)/config_parser_test.cc $(SRC_DIR)/config_parser.cc ${GTEST_DIR}/src/gtest_main.cc libgtest.a -lboost_system -o $(TEST_DIR)/config_parser_test

$(TEST_DIR)/config_handler_test: $(SRC_DIR)/config_handler.cc $(TEST_DIR)/config_handler_test.cc
	g++ -std=c++0x -isystem ${GTEST_DIR}/include -pthread $(TEST_DIR)/config_handler_test.cc $(SRC_DIR)/config_handler.cc $(SRC_DIR)/config_parser.cc ${GTEST_DIR}/src/gtest_main.cc libgtest.a -lboost_system -o $(TEST_DIR)/config_handler_test

$(TEST_DIR)/server_test: $(TEST_DIR)/server_test.cc 
	g++ -c -I${GTEST_DIR}/include/ -I${GMOCK_DIR}/include $(TEST_DIR)/server_test.cc ${GTEST_DIR}/src/gtest_main.cc
	g++ -o $(TEST_DIR)/server_test gtest_main.o server_test.o -L./ -lgmock -lgtest -lboost_system -lpthread





