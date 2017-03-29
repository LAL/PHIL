# -*- coding: utf-8 -*-
"""
Created on Mon Mar 06 20:41:42 2017

Demo for pywavecatcher

@author: $delerue
"""

from pyWavecatcher import *
       
from ctypes import *
import sys,ctypes
import os
import time
       
       
nDevices, deviceDescriptor, deviceSerNumChar = pyWAVECAT64CH_GetWaveCatDevices()
print nDevices
print repr(deviceDescriptor[0].value)
print repr(deviceSerNumChar[0].value)

print nDevices.value

if nDevices.value!=1:
    print nDevices
    print "Incorrect number of devices"
    sys.exit(-1)

    
errCode=0
errCode += pyWAVECAT64CH_OpenDevice()
checkError(errCode)
#WAVECAT64CH_GetDeviceInfo
errCode += pyWAVECAT64CH_ResetDevice()
checkError(errCode)
errCode += pyWAVECAT64CH_AllocateEventStructure()  
checkError(errCode)
(errCode,triggerType)=pyWAVECAT64CH_GetTriggerMode()
print "Trigger type"
print triggerType
checkError(errCode)
errCode +=pyWAVECAT64CH_SetTriggerMode(2)
checkError(errCode)
(errCode,triggerType)=pyWAVECAT64CH_GetTriggerMode()
checkError(errCode)
print "Trigger type"
print triggerType
errCode += pyWAVECAT64CH_StartRun()   
checkError(errCode)
time.sleep(1)
errCode += pyWAVECAT64CH_PrepareEvent()
checkError(errCode)
errCode += pyWAVECAT64CH_ReadEventBuffer()
checkError(errCode)
time.sleep(0.01)
errCode += pyWAVECAT64CH_DecodeEvent()
print repr(theEvent._fields_)
print repr(theEvent.ChannelData)
print repr(theEvent.ChannelData[0].WaveformDataSize)
print repr(theEvent.ChannelData[0].WaveformData)
for ichan in range(1,7):
    txt=""
    for idata in range(1,5):
        txt=txt+" "+str(theEvent.ChannelData[ichan].WaveformData[idata])
    print txt
	
time.sleep(1)
errCode += pyWAVECAT64CH_PrepareEvent()
checkError(errCode)
errCode += pyWAVECAT64CH_ReadEventBuffer()
checkError(errCode)
time.sleep(0.01)
errCode += pyWAVECAT64CH_DecodeEvent()
print repr(theEvent._fields_)
print repr(theEvent.ChannelData)
print repr(theEvent.ChannelData[0].WaveformDataSize)
print repr(theEvent.ChannelData[0].WaveformData)
for ichan in range(1,7):
    txt=""
    for idata in range(1,5):
        txt=txt+" "+str(theEvent.ChannelData[ichan].WaveformData[idata])
    print txt
#	if(errCode == WAVECAT64CH_CommError) printf("Communication Error\n"); 
#}
		
checkError(errCode)
errCode = pyWAVECAT64CH_StopRun();
checkError(errCode)
errCode = pyWAVECAT64CH_CloseDevice();
checkError(errCode)

pyWAVECAT64CH_CloseDevice()
checkError(errCode)

print "end"
print errCode


