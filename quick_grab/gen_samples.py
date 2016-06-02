#!/usr/bin/python

import sys
import numpy as np
import peregrine.samples

dataFile = sys.argv[1]



with open(dataFile, 'r') as f:
	content = f.readlines()

last = 100
lineCount=0
data=np.zeros(len(content),dtype=np.int8);
for line in content:
	val = int(line,16)
	check=val>>4
	if not last==100 and not (last+1)%13 == check:
		print(val,check,last)
		print("check failed",lineCount)
		exit(1)
	last = check
	data[lineCount]=val&0xF
	if not data[lineCount]==0 and not data[lineCount]==1:
		print("bad data",lineCount)
		exit(1)
	if data[lineCount]==0:
		data[lineCount]=-1
	lineCount=lineCount+1

data=data[0:len(data)/8*8]

peregrine.samples.save_samples("samples_file.piksi_format", data,file_format='piksi')


