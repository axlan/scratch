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

fSamp = 16.368e6

simDopFreq = 4.092e6+3e3

codeStartOffsetSamp = 1000 

fftSize = 2**17


chipsPerSample = nominalCodeRate / fSamp

simCode = CaCodeGen.genCode(svID,chipsPerSample,fftSize+codeStartOffsetSamp)
simCode = simCode[codeStartOffsetSamp:]

simDop = CarrierGen.genCarrier(fSamp,simDopFreq,fftSize)

simReplica = np.array(simDop) * np.array(simCode)

simReplicaQuant = np.array(simReplica,dtype=np.int8)
idx=simReplica>=0
simReplicaQuant[idx]=1
idx=simReplica<0
simReplicaQuant[idx]=-1

import peregrine.samples
peregrine.samples.save_samples("sim_gen_file.piksi_format", simReplicaQuant,file_format='piksi')


