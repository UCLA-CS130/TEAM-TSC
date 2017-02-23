#A simple web server.
A web server written in C++ using Boost Lib. 

##SetUp
###Installation
```
$ git clone --recursive https://github.com/UCLA-CS130/TSC.git
```
###Dependencies
####Boost
```
$ sudo apt-get install libboost-all-dev
```
###Usage
To run web-server
```
$ make
$ ./build/webserver src/config_file
```
To test (including unit-tests and integration-test)
```
$ make test
```
To see test-coverage
```
$ make test-coverage
```
##Source Code layout
- src : Source code for the webserver
 - Server : Runs the server and accepts connections
 - Connection : Handles connection-related events (reading and writing)
 - ConfigHandler : Encapsulates config parsing and access to config options
 - ConfigParser : Parses the config
 - Request and Response : Classes that represent parsed requests and responses
 - RequestHandler : Parent to the webserver handlers that defines shared functionality
 - EchoHandler : Echo handling
 - StaticFileHandler : Static file handling
- test : Contains tests for each class
- build : build files
- content1 and content2 : webserver's static directories

##RequestHandlers
- EchoHandler : Echo client request.
- StaticFileHandler : Return static file to client.
- StatusHandler : Show the webserver status including details of request received and service provide.
- ErrorHandler : Handle different HTTP errors and show details.



##Contributors
- Thomas Chang
- Xue Sun
- Xin Xu
