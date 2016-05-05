# -*- coding: utf-8 -*-
"""
Created on Wed May 04 15:01:43 2016

@author: jdiamond
"""
import math

class CarrierGen:

    @staticmethod
    def genCarrier(fSamp,freq,numberOfSamples):
        carrier = []
        t = 0
        T=1/fSamp
        for i in xrange(numberOfSamples):
            carrier.append(math.cos(t*2*math.pi*freq))
            t += T
        return carrier