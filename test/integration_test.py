import subprocess
import os
import shutil

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
    path /proxy1 ProxyHandler {\
        host www.google.com;\
        port 80;\
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

print(bcolors.OKBLUE + '[----------] ' + bcolors.ENDC + 'send proxy request to server by curl')

#PROXY TESTS---------------------------------------------------------------------- 
request_proxy = 'curl -i localhost:8080/proxy1/'
curl_proc = subprocess.Popen(request_proxy, stdout=subprocess.PIPE, shell=True)
response_proxy = curl_proc.stdout.read()

expected_response_proxy = 'TODO'



webserver.kill()
shutil.rmtree(TMP_FILE_DIR)

print(bcolors.OKBLUE + '[----------] ' + bcolors.ENDC +'check the results of echo, static, and proxy tests')

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
        exit(0)

if response_proxy != expected_response_proxy:
        print(bcolors.FAIL + '[   FAIL   ] ' + bcolors.ENDC + 'Incorrect Reply in Proxy Test!')
        print('Expected: ' + str(len(expected_response_proxy)) + '\n' + expected_response_proxy)
        print('Response: ' + str(len(response_proxy)) + '\n' + response_proxy)
        exit(1)
else:
        print(bcolors.OKBLUE + '[ SUCCESS! ] ' + bcolors.ENDC + 'Proxy Test Succeeded!')
        exit(0)
