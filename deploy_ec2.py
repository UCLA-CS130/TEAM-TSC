#!/usr/bin/env python
import sys
import spur
import spur.ssh
import signal
import os

if (len(sys.argv) < 4):
	print "Usage python deploy_aws.py <pem_file> <image_name> <ec2_host>"
	exit(1)

pem_file = sys.argv[1]
image_name = sys.argv[2]
ec2_host = sys.argv[3]
ec2_home_dir = "ec2-user@{0}:~".format(ec2_host)
image_tar = image_name + ".tar"

local_shell = spur.LocalShell()
local_shell.run(["docker", "save", "-o", image_tar, image_name])
local_shell.run(["scp", "-i", pem_file, image_tar, ec2_home_dir])

local_shell.run(["rm", "-f", image_tar])

ec2_shell = spur.SshShell(
	hostname=ec2_host,
	username="ec2-user",
	private_key_file=pem_file,
	missing_host_key=spur.ssh.MissingHostKey.accept
)


# kill the running server container
'''
running_server_container_id_ = ec2_shell.run(["docker", "ps", "-q", "-f", "ancestor=" + image_name])
running_server_container_id = running_server_container_id_.output
if running_server_container_id:
	print running_server_container_id
	ec2_shell.run(["docker", "kill", running_server_container_id])
	print ("killing and the running server container: ", running_server_container_id)

# delete the old image
old_image_id_ = ec2_shell.run(["docker", "images", image_name, "-q"])
old_image_id = old_image_id_.output
if old_image_id:
	ec2_shell.run(["docker", "rmi", old_image_id])
	print ("delete the old server image: ", old_image_id)
'''

# create the new server image
ec2_shell.run(["docker", "load", "-i", image_tar])
ec2_shell.run(["rm", "-f", image_tar])
show_images = ec2_shell.run(["docker", "images"])
print show_images.output

ec2_shell.spawn(["docker", "run", "--rm", "-t", "-p", "8080:8080", image_name], store_pid=True)
exit(1)