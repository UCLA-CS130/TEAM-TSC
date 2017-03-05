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

CONFIG_FILE_DIR = 'src'

SCRIPT_PATH = os.path.dirname(__file__)
EXE_PATH = os.path.join(SCRIPT_PATH, '..', 'build', 'webserver')


print(bcolors.OKBLUE + '[==========] ' + bcolors.ENDC + 'start the webserver')

webserver = subprocess.Popen([EXE_PATH, CONFIG_FILE_DIR + '/config_file'])

print(bcolors.OKBLUE + '[----------] ' + bcolors.ENDC + 'send a request to server by curl')

request = 'curl -i localhost:8080/'
curl_proc = subprocess.Popen(request, stdout=subprocess.PIPE, shell=True)
response = curl_proc.stdout.read().decode('utf-8')

response = response.split('\r\n')
if response[0] == "HTTP/1.0 200 OK":
	print "Redirect integration test passed"
else:
	print "Redirect integration test failed"
	exit(1)

webserver.kill()


