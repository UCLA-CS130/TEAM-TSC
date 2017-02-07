import subprocess
import os

#print('make clean and make')
#subprocess.check_call('make clean; make', stdout=subprocess.PIPE, shell=True)


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

print(bcolors.OKBLUE + '[==========] ' + bcolors.ENDC + 'start the webserver')
wr = open('src/integration_config_file', 'w')
config_contents = 'server {\
listen 8080;\
handler_map echo_handler /echo;\
handler_map static_handler /static1 /static2;\
base_path /static1 content1;\
base_path /static2 content2;\
}'
wr.write(config_contents)
wr.close()

webserver = subprocess.Popen(['./build/webserver', 'src/integration_config_file'])

print(bcolors.OKBLUE + '[----------] ' + bcolors.ENDC + 'send echo request to server by telnet')
#ECHO TESTS----------------------------------------------------------------------
request = 'curl -i localhost:8080/echo'
telnet_proc = subprocess.Popen(request, stdout=subprocess.PIPE, shell=True)

response = telnet_proc.stdout.read().decode('utf-8')

expected_response = 'HTTP/1.0 200 OK\r\n\
Content-Length: 82\r\n\
Content-Type: text/plain\r\n\r\n\
GET /echo HTTP/1.1\r\n\
User-Agent: curl/7.35.0\r\n\
Host: localhost:8080\r\n\
Accept: */*\r\n\r\n'

print(bcolors.OKBLUE + '[----------] ' + bcolors.ENDC + 'send static request to server by telnet')

#STATIC TESTS---------------------------------------------------------------------- 
request_static = 'curl -i localhost:8080/static1/test.html'
telnet_proc = subprocess.Popen(request_static, stdout=subprocess.PIPE, shell=True)

response_static = telnet_proc.stdout.read().decode('utf-8')

expected_response_static = 'HTTP/1.0 200 OK\r\n\
Content-Length: 78\r\n\
Content-Type: text/html\r\n\r\n\
<html>\n\
  <head><title>hello</title></head>\n\
  <body><h1>TSC</h1></body>\n\
</html>'

wr = open('static_test', 'w')
wr.write(expected_response_static + '\n' + response_static)
wr.close()

webserver.kill()

print(bcolors.OKBLUE + '[----------] ' + bcolors.ENDC +'check the results of echo and static tests')

'''
diff = []
for index in range(len(response)):
	if expected_response[index] != response[index]:
		diff.append(response[index])
print diff
'''

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
