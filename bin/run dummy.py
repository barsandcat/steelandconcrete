#!/usr/bin/python
import os

for i in range(1000):
	os.spawnlp(os.P_NOWAIT, "./dummy");

