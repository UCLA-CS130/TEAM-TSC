CXX=g++
CXXFLAGS= -g -Wall -pthread -std=c++0x -lboost_system
TESTFLAGS= -std=c++11 -isystem
TESTARGS= -pthread

GTEST_DIR=googletest/googletest
GMOCK_DIR=googletest/googlemock
TEST_DIR=unit_tests
SRC_DIR=src
BUILD_DIR=build

CCFILE = src/*.cc
DEPS = src/*.h

all: webserver

webserver: $(CCFILE) $(DEPS)
	$(CXX) -o $(BUILD_DIR)/$@ $(CCFILE) $(CXXFLAGS)

.PHONY: clean test

#test: unit_test integration_test

integration_test: webserver
	python integration_test.py

unit_test: gtest_setup connection_test config_parser_test config_handler_test server_test

gtest_setup:
	g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    	-pthread -c ${GTEST_DIR}/src/gtest-all.cc
	ar -rv $(BUILD_DIR)/libgtest.a gtest-all.o

	g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
    	-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
    	-pthread -c ${GMOCK_DIR}/src/gmock-all.cc
	ar -rv $(BUILD_DIR)/libgmock.a gtest-all.o gmock-all.o
	rm gtest-all.o gmock-all.o

connection_test: $(TEST_DIR)/connection_test.cc $(SRC_DIR)/connection.cc
	$(CXX) $(TESTFLAGS) ${GTEST_DIR}/include $(TESTARGS) $^ ${GTEST_DIR}/src/gtest_main.cc $(BUILD_DIR)/libgtest.a -lboost_system -o $(BUILD_DIR)/$@

config_parser_test: $(TEST_DIR)/config_parser_test.cc $(SRC_DIR)/config_parser.cc
	$(CXX) $(TESTFLAGS) ${GTEST_DIR}/include $(TESTARGS) $^ ${GTEST_DIR}/src/gtest_main.cc $(BUILD_DIR)/libgtest.a -lboost_system -o $(BUILD_DIR)/$@

config_handler_test: $(TEST_DIR)/config_handler_test.cc $(SRC_DIR)/config_handler.cc $(SRC_DIR)/config_parser.cc
	$(CXX) $(TESTFLAGS) ${GTEST_DIR}/include $(TESTARGS) $^ ${GTEST_DIR}/src/gtest_main.cc $(BUILD_DIR)/libgtest.a -lboost_system -o $(BUILD_DIR)/$@

server_test: $(TEST_DIR)/server_test.cc 
	g++ -c -I${GTEST_DIR}/include -I${GMOCK_DIR}/include $(TEST_DIR)/server_test.cc ${GTEST_DIR}/src/gtest_main.cc
	g++ -o $(BUILD_DIR)/$@ gtest_main.o server_test.o -L./ -lgmock -lgtest -lboost_system -lpthread
	rm server_test.o gtest_main.o

# bug!
test_coverage: TESTARGS += -fprofile-arcs -ftest-coverage
test_coverage: unit_test connection_test_coverage config_parser_test_coverage config_handler_test_coverage 

connection_test_coverage: connection_test
	gcov -o . -r $(SRC_DIR)/connection.cc

config_parser_test_coverage: config_parser_test
	gcov -o . -r $(SRC_DIR)/config_parser.cc

config_handler_test_coverage: config_handler_test
	gcov -o . -r $(SRC_DIR)/config_handler.cc

#server_test: $(TEST_DIR)/server_test.cc 
#	g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
#    	-pthread -c $(TEST_DIR)/server_test.cc ${GTEST_DIR}/src/gtest_main.cc 
#    g++ -o $(BUILD_DIR)/server_test gtest_main.o server_test.o -L./ -lgmock -lgtest -lboost_system -lpthread
#    rm server_test.o gtest_main.o

#server_test: $(TEST_DIR)/server_test.cc
#	$(CXX) $(TESTFLAGS) -I${GMOCK_DIR}/include -I${GTEST_DIR}/include -pthread $^ ${GTEST_DIR}/src/gtest_main.cc libgmock.a libgtest.a -lboost_system -o $(BUILD_DIR)/$@

clean:
	rm -rf $(BUILD_DIR)/* *.o *.gcno *.gcov *.gcda



