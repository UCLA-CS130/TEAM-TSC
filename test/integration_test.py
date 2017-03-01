import subprocess
import os
import shutil
import sys
import telnetlib

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

TMP_FILE_DIR = 'tmp_files'
os.makedirs(TMP_FILE_DIR)

SCRIPT_PATH = os.path.dirname(__file__)
EXE_PATH = os.path.join(SCRIPT_PATH, '..', 'build', 'webserver')

print(bcolors.OKBLUE + '[==========] ' + bcolors.ENDC + 'start the webserver')
wr = open(TMP_FILE_DIR + '/config_file', 'w')
config_contents = '\
server {\
    port 8080;\
    path /echo EchoHandler;\
    path /static1 StaticHandler {\
        root content1;\
    }\
    path /static2 StaticHandler {\
        root content2;\
    }\
}'
wr.write(config_contents)
wr.close()

webserver = subprocess.Popen([EXE_PATH, TMP_FILE_DIR + '/config_file'])

print(bcolors.OKBLUE + '[----------] ' + bcolors.ENDC + 'send echo request to server by curl')
#ECHO TESTS----------------------------------------------------------------------
request = 'curl -i localhost:8080/echo'
curl_proc = subprocess.Popen(request, stdout=subprocess.PIPE, shell=True)
response = curl_proc.stdout.read().decode('utf-8')

expected_response = 'HTTP/1.0 200 OK\r\n\
Content-Length: 82\r\n\
Content-Type: text/plain\r\n\r\n\
GET /echo HTTP/1.1\r\n\
User-Agent: curl/7.35.0\r\n\
Host: localhost:8080\r\n\
Accept: */*\r\n\r\n'

print(bcolors.OKBLUE + '[----------] ' + bcolors.ENDC + 'send static request to server by curl')

#STATIC TESTS---------------------------------------------------------------------- 
request_static = 'curl -i localhost:8080/static1/test.html'
curl_proc = subprocess.Popen(request_static, stdout=subprocess.PIPE, shell=True)
response_static = curl_proc.stdout.read().decode('utf-8')

expected_response_static = 'HTTP/1.0 200 OK\r\n\
Content-Length: 78\r\n\
Content-Type: text/html\r\n\r\n\
<html>\n\
  <head><title>hello</title></head>\n\
  <body><h1>TSC</h1></body>\n\
</html>'


#MULTITHREAD TESTS------------------------------------------------------------------
host = "localhost"
expected_response_thread = 'HTTP/1.0 200 OK\r\n\
Content-Length: 77\r\n\
Content-Type: text/plain\r\n\r\n\
GET /echo HTTP/1.1\r\n\
User-Agent: telnet\r\n\
Host: localhost:8080\r\n\
Accept: */*\r\n\r\n'

first_half_message = "GET /echo HTTP/1.1\r\n\
User-Agent: telnet\r\n\
Host: localhost:8080\r\n"
second_half_message = "Accept: */*\r\n\r\n"

tn1 = telnetlib.Telnet(host, 8080, 5)
tn1.write(first_half_message)
tn1_response = tn1.read_eager() 

tn2 = telnetlib.Telnet(host, 8080, 5)
tn2.write(first_half_message + second_half_message)
tn2_response = tn2.read_all()

tn1.write(second_half_message)
tn1_response = tn1_response + tn1.read_all()

webserver.kill()
shutil.rmtree(TMP_FILE_DIR)

print(bcolors.OKBLUE + '[----------] ' + bcolors.ENDC +'check the results of echo and static and multithread tests')

if response != expected_response:
	print(bcolors.FAIL + '[   FAIL   ] ' + bcolors.ENDC + 'Incorrect Reply in Echo Test!')
	print('Expected: ' + str(len(expected_response)) + '\n' + expected_response)
	print('Response: ' + str(len(response)) + '\n' + response)
else:
	print(bcolors.OKBLUE + '[ SUCCESS! ] ' + bcolors.ENDC + 'Echo Test Succeeded!')

if response_static != expected_response_static:
        print(bcolors.FAIL + '[   FAIL   ] ' + bcolors.ENDC + 'Incorrect Reply in Static Test!')
        print('Expected: ' + str(len(expected_response_static)) + '\n' + expected_response_static)
        print('Response: ' + str(len(response_static)) + '\n' + response_static)
        exit(1)
else:
        print(bcolors.OKBLUE + '[ SUCCESS! ] ' + bcolors.ENDC + 'Static Test Succeeded!')
        

if tn1_response != expected_response_thread or tn2_response != expected_response_thread:
    print(bcolors.FAIL + '[   FAIL   ] ' + bcolors.ENDC + "Incorrect Reply in multithread test!")
    print("Expected first response: " + str(len(expected_response_thread)) + '\n' + expected_response_thread)
    print("First response: " + str(len(tn1_response)) + '\n' + tn1_response)
    print("Second response: " + str(len(tn2_response)) + '\n' + tn2_response)
    exit(1)
else:
    print(bcolors.OKBLUE + '[ SUCCESS! ] ' + bcolors.ENDC + "Multithread Test Succeeded!")
    exit(0)
