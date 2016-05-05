# -*- coding: utf-8 -*-
"""
Created on Wed May 04 14:55:50 2016

@author: jdiamond
"""

# Sort of works, since it's real negative frequencies don'treally make sense

from CaCodeGen import CaCodeGen
from CarrierGen import CarrierGen

import numpy as np
import matplotlib.pyplot as plt


#Constants

nominalCodeRate=1023*1e3


#Parameters

svID = 1

fSamp = 8e6

simDopFreq = 3e3

codeStartOffsetSamp = 1000 

fftSize = 2**15

searchDopplerMin = -5e3
searchDopplerMax = 5e3
searchDopplerStep = 200

chipsPerSample = nominalCodeRate / fSamp

simCode = CaCodeGen.genCode(svID,chipsPerSample,fftSize+codeStartOffsetSamp)
simCode = simCode[codeStartOffsetSamp:]

simDop = CarrierGen.genCarrier(fSamp,simDopFreq,fftSize)

simReplica = np.array(simDop) * np.array(simCode)

#plt.plot(simReplica)
#plt.show()

searchCode = CaCodeGen.genCode(svID,chipsPerSample,fftSize);

simFFT = np.fft.fft(simReplica)
searchFFT = np.fft.fft(searchCode)


fftBinWidth = fSamp/fftSize

bestResultVal=0
bestResultDoppler=0
bestResultOffset=0

for searchDopplerBin in range(int(searchDopplerMin/searchDopplerStep),int(searchDopplerMax/searchDopplerStep)):    
    doppler=searchDopplerBin * searchDopplerStep;    
    sampleOffset = np.round(doppler / fftBinWidth)
    if sampleOffset < 0:
        sampleOffset = fftSize + sampleOffset
    result_fft=[]
    for i in range(fftSize):
        a = searchFFT[i]
        b = simFFT[(i+sampleOffset)%fftSize]
        #val=(a.real*b.real + a.imag*b.imag) + 1j * (a.real * -b.imag + a.imag * b.real)
        val= a * b.conj()
        result_fft.append(val)
     
    results = np.abs(np.fft.ifft(result_fft))
    
    tempBestIdx=np.argmax(results)
    tempBest=results[tempBestIdx]
    if tempBest>bestResultVal:
        bestResultDoppler=doppler
        bestResultOffset=tempBestIdx
        bestResultVal=tempBest
        
print('dop:%d,idx:%d'%(bestResultDoppler,bestResultOffset))

#plt.plot(results)
#plt.show()
#result=[]    
#for i in range(8000):
#    shiftedCode = np.concatenate((np.array(searchCode[i:]),np.zeros(i)))
#    multResult = shiftedCode * np.array(simReplica)
#    result.append(np.sum(multResult))
#
#plt.plot(result)    
#plt.show()
