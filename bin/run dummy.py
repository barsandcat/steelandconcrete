#!/usr/bin/python
import os
import subprocess
import shutil

if os.path.exists("log"):
	shutil.rmtree("log");
os.mkdir("log");

for i in range(1000):
	f = open("log/client" + str(i) + ".log", "w");
	subprocess.Popen("./dummy", stdout=f, stderr=subprocess.STDOUT, close_fds=True)


