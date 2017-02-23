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
- src
- test

##RequestHanlders
- EchoHandler : Echo client request.
- StaticFileHandler : Return static file to client.
- StatusHandler : Show the webserver status including details of request received and service provide.
- ErrorHandler : Handle different HTTP errors and show details.

##Contributors
- Thomas Chang
- Xue Sun
- Xin Xu
