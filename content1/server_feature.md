
# A Simple Web Server -- Team TSC

## Features:
* ### Markdown Rendering
* ### Mysql Databse Interface
* ### A Python Application Server
* ### Load Testing
* ### HTTP Compression
* ### Keep-alive Connection
* ### A Cache Proxy

<br>
----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
## Markdown Rendering
Using Cpp-Markdown lib.<br>
github: [https://github.com/sevenjay/cpp-markdown](https://github.com/sevenjay/cpp-markdown)

<br>
----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
## Mysql Databse Interface
* Using libmysqlcppconn-dev.
* You can query the database, and add and delete rows from tables.
* demo: [http://35.165.85.42:8080/static1/database_test.html](http://35.165.85.42:8080/static1/database_test.html)

<br>
----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
## A Python Application Server
* A PythonHandler to call python scripts and handle the data conversion between c++ and python.
* Python scripts can interact with database directly and generate dynamic contents.
* You can extend the python web server easily by adding python scripts in folder -- python-bin
* The main logic:

<br>
  ![python-app](python_app.png)

<br><br>
----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
## Load Testing
* Use Tsung for load testing.
* Performance before optimization:
* Performance after optimization ( Proxy Cache + HTTP Compression + Keep-alive Connection):


<br><br>
----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
## HTTP Compression

<br><br>
----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
## Keep-alive Connection

<br><br>
----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
## A Cache Proxy
![proxy-cache](proxy_cache.png)
