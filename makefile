GTEST_DIR=googletest/googletest
GMOCK_DIR=googletest/googlemock
TEST_DIR=test
SRC_DIR=src
BUILD_DIR=build

CXX =g++
PYTHON_LDFLAG=-ldl -lutil -lm -lpython2.7 -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions
PYTHON_CFLAG=-fno-strict-aliasing -D_FORTIFY_SOURCE=2 -g -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security  -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes
CXXFLAGS=-g -Wall -std=c++11 -DBOOST_LOG_DYN_LINK 
CXXLINK=-pthread -lmysqlcppconn -lboost_system -lboost_log -lboost_regex -lboost_filesystem -lboost_thread -lpython2.7 -DBOOST_LOG_DYN_LINK 
#CXXFLAGS =-g -Wall -std=c++11 
#CXXLINK =-static-libgcc -static-libstdc++ -pthread -lmysqlcppconn -Wl,-Bstatic -lboost_system -lboost_log_setup -lboost_log -lboost_regex -lboost_filesystem -lboost_thread
TESTFLAGS =-std=c++11 -isystem ${GTEST_DIR}/include -isystem ${GMOCK_DIR}/include -DBOOST_LOG_DYN_LINK
TESTARGS =-pthread
TESTLINK =-L./build/ -lgmock -lgtest -lboost_system -lboost_log -lboost_regex -lboost_filesystem -lpthread

SRCFILE = src/*.cc cpp-markdown/*.cpp
DEPS = src/*.h cpp-markdown/*.h

all: create_table webserver

create_table:
	mysql -u root < $(SRC_DIR)/create_table.sql

webserver: $(SRCFILE) $(DEPS) 
	$(CXX) -o $(BUILD_DIR)/$@ $(SRCFILE) $(CXXFLAGS) $(CXXLINK)

.PHONY: clean test

test: unit_test integration_test

integration_test: webserver
	python $(TEST_DIR)/integration_test.py
	python $(TEST_DIR)/integration_test_redirect.py
	python $(TEST_DIR)/threading_integration_test.py 2

unit_test: gtest_setup connection_test config_parser_test config_handler_test static_file_handler_test echo_handler_test request_test response_test proxy_handler_test

	./$(BUILD_DIR)/connection_test;\
	./$(BUILD_DIR)/config_parser_test;\
	./$(BUILD_DIR)/config_handler_test;\
	./$(BUILD_DIR)/static_file_handler_test;\
	./$(BUILD_DIR)/echo_handler_test;\
	./$(BUILD_DIR)/request_handler_test;\
	./$(BUILD_DIR)/request_test;\
	./$(BUILD_DIR)/response_test;\
	./$(BUILD_DIR)/proxy_handler_test


gtest_setup:
	g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
	-pthread -c ${GTEST_DIR}/src/gtest-all.cc
	ar -rv $(BUILD_DIR)/libgtest.a gtest-all.o

	g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
	-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
	-pthread -c ${GMOCK_DIR}/src/gmock-all.cc
	ar -rv $(BUILD_DIR)/libgmock.a gtest-all.o gmock-all.o
	rm gtest-all.o gmock-all.o


connection_test: $(TEST_DIR)/connection_test.cc $(SRC_DIR)/connection.cc $(SRC_DIR)/request_handler.cc $(SRC_DIR)/server_status.cc $(SRC_DIR)/request.cc $(SRC_DIR)/response.cc
	$(CXX) $(TESTFLAGS) $(TESTARGS) $^ ${GTEST_DIR}/src/gtest_main.cc $(TESTLINK) -o $(BUILD_DIR)/$@

config_parser_test: $(TEST_DIR)/config_parser_test.cc $(SRC_DIR)/config_parser.cc
	$(CXX) $(TESTFLAGS) $(TESTARGS) $^ ${GTEST_DIR}/src/gtest_main.cc $(TESTLINK) -o $(BUILD_DIR)/$@

config_handler_test: $(TEST_DIR)/config_handler_test.cc $(SRC_DIR)/config_handler.cc 
	$(CXX) $(TESTFLAGS) $(TESTARGS) $^ ${GTEST_DIR}/src/gtest_main.cc $(TESTLINK) -o $(BUILD_DIR)/$@


request_handler_test: $(TEST_DIR)/request_handler_test.cc $(SRC_DIR)/request_handler.cc $(SRC_DIR)/request.cc $(SRC_DIR)/response.cc 
	$(CXX) $(TESTFLAGS) $(TESTARGS) $^ ${GTEST_DIR}/src/gtest_main.cc $(TESTLINK) -o $(BUILD_DIR)/$@

static_file_handler_test: $(TEST_DIR)/static_file_handler_test.cc $(SRC_DIR)/static_file_handler.cc $(SRC_DIR)/request_handler.cc $(SRC_DIR)/request.cc $(SRC_DIR)/response.cc cpp-markdown/*.cpp
	$(CXX) $(TESTFLAGS) $(TESTARGS) $^ ${GTEST_DIR}/src/gtest_main.cc $(TESTLINK) -o $(BUILD_DIR)/$@

echo_handler_test: $(TEST_DIR)/echo_handler_test.cc $(SRC_DIR)/echo_handler.cc $(SRC_DIR)/request_handler.cc $(SRC_DIR)/request.cc $(SRC_DIR)/response.cc 
	$(CXX) $(TESTFLAGS) $(TESTARGS) $^ ${GTEST_DIR}/src/gtest_main.cc $(TESTLINK) -o $(BUILD_DIR)/$@

request_test: $(TEST_DIR)/request_test.cc $(SRC_DIR)/request.cc
	$(CXX) $(TESTFLAGS) $(TESTARGS) $^ ${GTEST_DIR}/src/gtest_main.cc $(TESTLINK) -o $(BUILD_DIR)/$@

response_test: $(TEST_DIR)/response_test.cc $(SRC_DIR)/response.cc
	$(CXX) $(TESTFLAGS) $(TESTARGS) $^ ${GTEST_DIR}/src/gtest_main.cc $(TESTLINK) -o $(BUILD_DIR)/$@

proxy_handler_test: $(TEST_DIR)/proxy_handler_test.cc $(SRC_DIR)/proxy_handler.cc $(SRC_DIR)/request_handler.cc $(SRC_DIR)/request.cc $(SRC_DIR)/response.cc
	$(CXX) $(TESTFLAGS) $(TESTARGS) $^ $(GTEST_DIR)/src/gtest_main.cc $(TESTLINK) -o $(BUILD_DIR)/$@


test_coverage: TESTARGS += -fprofile-arcs -ftest-coverage

test_coverage: gtest_setup connection_test config_parser_test config_handler_test static_file_handler_test echo_handler_test request_test response_test proxy_handler_test
	./$(BUILD_DIR)/connection_test && gcov -r connection.cc;\
	./$(BUILD_DIR)/config_parser_test && gcov -r config_parser.cc;\
	./$(BUILD_DIR)/config_handler_test && gcov -r config_handler.cc;\
	./$(BUILD_DIR)/static_file_handler_test && gcov -r static_filehandler.cc;\
	./$(BUILD_DIR)/echo_handler_test && gcov -r echo_handler.cc;\
	./$(BUILD_DIR)/request_test && gcov -r request_handler.cc;\
	./$(BUILD_DIR)/response_test && gcov -r response_handler.cc;\
	./$(BUILD_DIR)/proxy_handler_test && gcov -r proxy_handler.cc;\

docker:
	./docker.sh

deploy: docker
	python deploy_ec2.py ../../TSC.pem httpserver ec2-35-163-116-30.us-west-2.compute.amazonaws.com

load_test:
	python $(TEST_DIR)/load-testing.py ../../TSC.pem $(TEST_DIR)/load-testing.xml ec2-35-163-116-30.us-west-2.compute.amazonaws.com True

clean: 
	rm -rf $(BUILD_DIR)/* *.o *.a *.gcno *.gcov *.gcda



