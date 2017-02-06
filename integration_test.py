import subprocess
import os

#print('make clean and make')
#subprocess.check_call('make clean; make', stdout=subprocess.PIPE, shell=True)

print('start the webserver')
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

print('send request to server by telnet')

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

webserver.kill()

print('check the response and expected_response')

'''
diff = []
for index in range(len(response)):
	if expected_response[index] != response[index]:
		diff.append(response[index])
print diff
'''

if response != expected_response:
	print('Test Fail! Incorrect Reply!')
	print('Expected: ' + str(len(expected_response)) + '\n' + expected_response)
	print('Response: ' + str(len(response)) + '\n' + response)
	exit(1)
else:
	print('Test Succeed!')
	exit(0)
