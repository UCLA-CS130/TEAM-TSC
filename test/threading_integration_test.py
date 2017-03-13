import subprocess
import os
import shutil
import sys
import telnetlib
import time

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
    thread 10;\
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
time.sleep(5)


print(bcolors.OKBLUE + '[----------] ' + bcolors.ENDC + 'send echo request to server by curl')
#ECHO TESTS----------------------------------------------------------------------
request = 'curl -i localhost:8080/echo'
curl_proc = subprocess.Popen(request, stdout=subprocess.PIPE, shell=True)
response = curl_proc.stdout.read().decode('utf-8')


print(bcolors.OKBLUE + '[----------] ' + bcolors.ENDC + 'Begin testing for mutithreading')
#MULTITHREAD TESTS------------------------------------------------------------------
host = "localhost"
expected_response_thread = 'HTTP/1.1 200 OK\r\n\
Content-Length: 96\r\n\
Content-Type: text/plain\r\n\
Connection: close\r\n\r\n\
GET /echo HTTP/1.1\r\n\
User-Agent: telnet\r\n\
Host: localhost:8080\r\n\
Connection: close\r\n\
Accept: */*\r\n\r\n'

first_half_message = "GET /echo HTTP/1.1\r\n\
User-Agent: telnet\r\n\
Host: localhost:8080\r\n"
second_half_message = "Connection: close\r\n\
Accept: */*\r\n\r\n"

thread_num = int(sys.argv[1])
print (bcolors.OKBLUE + '[----------] ' + bcolors.ENDC +'total thread number : '+ str(thread_num))
tn_list  =[]
tn_response_list = []
for request_index in range(thread_num):
    print (bcolors.OKBLUE + '[----------] ' + bcolors.ENDC+"Send first of request index = "+ str(request_index))
    tn = telnetlib.Telnet(host, 8080, 5)
    tn.write(first_half_message)
    tn_response = tn.read_eager() 
    tn_list.append(tn)
    tn_response_list.append(tn_response)

for request_reverse_index in range(thread_num):
    request_index = thread_num-1-request_reverse_index
    print (bcolors.OKBLUE + '[----------] ' + bcolors.ENDC + "Send second half of request index = "+str(request_index))
    tn_list[request_index].write(second_half_message)
    tn_response_list[request_index] = tn_response_list[request_index] + tn_list[request_index].read_all()

webserver.kill()
shutil.rmtree(TMP_FILE_DIR)

print(bcolors.OKBLUE + '[----------] ' + bcolors.ENDC +'check the results of multithread tests')
        
for index in range(thread_num):
    if tn_response_list[index] != expected_response_thread:
        print(bcolors.FAIL + '[   FAIL   ] ' + bcolors.ENDC + "Incorrect Reply in multithread test!")
        print("Expected first response: " + str(len(expected_response_thread)) + '\n' + expected_response_thread)
        print("First response: " + str(len(tn1_response)) + '\n' + tn1_response)
        print("Second response: " + str(len(tn2_response)) + '\n' + tn2_response)
        exit(1)
else:
    print(bcolors.OKBLUE + '[ SUCCESS! ] ' + bcolors.ENDC + "Multithread Test Succeeded!")
    exit(0)
