#!/usr/bin/python

import sys
import numpy as np
import peregrine.samples

dataFile = sys.argv[1]

data=np.array([]);

with open(dataFile, 'r') as f:
	content = f.readlines()

last = 100
lineCount=1
for line in content:
	val = int(line,16)
	check=val>>8
	if not last==100 and not last == check:
		print("check failed",lineCount)
		exit(1)
	last = check
	data=np.append(data,[val&0xF])
	if not data[-1]==0 and not data[-1]==1:
		print("bad data",lineCount)
		exit(1)
	if data[-1]==0:
		data[-1]=-1
	lineCount=lineCount+1

peregrine.samples.save_samples("samples_file.dat", data)


