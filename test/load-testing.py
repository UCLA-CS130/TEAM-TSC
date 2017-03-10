#!/usr/bin/env python
import sys
import spur
import spur.ssh

if (len(sys.argv) < 4):
	print "Usage python load_testing <pem_file> <testing script> <tsung_host> <copy_down=False>"
	exit(1)

pem_file = sys.argv[1]
testing_script = sys.argv[2]
tsung_host = sys.argv[3]
tsung_home_dir = "ec2-user@{0}:~".format(tsung_host)
copy_down = sys.argv[4]

if len(sys.argv) == 5 and sys.argv[4] == "true":
	copy_down = True

local_shell = spur.LocalShell()
scp_result = local_shell.run(["scp", "-i", pem_file, testing_script, tsung_home_dir])
print scp_result.output

ec2_shell = spur.SshShell(
	hostname=tsung_host,
	username="ec2-user",
	private_key_file=pem_file,
	missing_host_key=spur.ssh.MissingHostKey.accept
)

testing_file_name = testing_script.split("/")[-1]
print testing_file_name
tsung_result = ec2_shell.run(["tsung", "-f", testing_file_name, "start"])
print tsung_result.output

log_dir = tsung_result.output.split(":")[1][1:-1]
print "Going to log_file {0}".format(log_dir)

stats_out = ec2_shell.run(["tsung_stats.pl"], cwd=log_dir)
print stats_out.output

if copy_down:
	local_shell.run(["scp", "-r", "-i", pem_file, "ec2-user@" + tsung_host + ":" + log_dir, "../load-test-resluts/."])