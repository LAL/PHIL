# -*- coding: utf-8 -*-
"""
Python interface for Wavecatchers
"""

from ctypes import *
import sys,ctypes
#import os
#import time

MAX_NUM_OF_DEVICES=8
MAX_DESCRIPTOR_SIZE=1024
SERNUM_SIZE=8
WAVECAT64CH_MAX_DATA_SIZE=1024
WAVECAT64CH_INFO_NOT_AVAILABLE=-1
WAVECAT64CH_NB_OF_CHANNELS_IN_SAMBLOCK=2
WAVECAT64CH_MAX_NB_OF_FE_BOARDS=4
WAVECAT64CH_NB_OF_CHANNELS_IN_SAMBLOCK=2
WAVECAT64CH_MAX_TOTAL_NB_OF_SAMBLOCKS=36
WAVECAT64CH_MAX_TOTAL_NB_OF_CHANNELS=72
WAVECAT64CH_ADCTOVOLTS=0.00061

#class FILE(ctypes.Structure):
#    pass

DeviceHandle=ctypes.POINTER(ctypes.c_int)

# Load DLL into memory.

wcDll = ctypes.CDLL ("C:\\work_delerue\\PHIL\\Wavecatcher\\python\\WaveCat64Ch.dll")


#Data structures
class ChannelDataStruct(Structure):
    _fields_ = [("ChannelType", c_uint),
                ("Channel", c_int),
                ("TrigCount", c_int),
                ("TimeCount", c_int),
                ("WaveformDataSize", c_int),
                ("WaveformData", POINTER(c_float)),
                ("Baseline", c_float),
                ("Peak",c_float),
                ("PeakCell",c_int),
                ("Charge",c_float),
                ("ChargeMeasureOverflow",c_bool ),
                ("CFDRisingEdgeTime",c_float),
                ("CFDFallingEdgeTime", c_float),
                ("FCR", c_int)
                ]

class Event(Structure):
    _fields_ = [("EventID", c_int),
                ("TDC", c_ulonglong),
                ("ChannelData", POINTER(ChannelDataStruct)),
                ("NbOfSAMBlocksInEvent", c_int)]

#Memory allocation 
theEvent=Event()


#Python wrapper for Wavecatcher functions
def pyWAVECAT64CH_GetWaveCatDevices():
    nDevices=ctypes.c_int(-1)
    deviceDescriptor=(ctypes.c_char*MAX_DESCRIPTOR_SIZE*MAX_NUM_OF_DEVICES)()
    deviceSerNumChar=(ctypes.c_char*MAX_NUM_OF_DEVICES*SERNUM_SIZE)()
    wcDll.WAVECAT64CH_GetWaveCatDevices(ctypes.byref(deviceDescriptor),ctypes.byref(deviceSerNumChar),ctypes.byref(nDevices))
    return nDevices, deviceDescriptor, deviceSerNumChar

def pyWAVECAT64CH_OpenDevice():
    return wcDll.WAVECAT64CH_OpenDevice()

def pyWAVECAT64CH_CloseDevice():
    return wcDll.WAVECAT64CH_CloseDevice()

def pyWAVECAT64CH_ResetDevice():
    return wcDll.WAVECAT64CH_ResetDevice()
    
def pyWAVECAT64CH_StartRun():
    return wcDll.WAVECAT64CH_StartRun()

def pyWAVECAT64CH_PrepareEvent():
    return wcDll.WAVECAT64CH_PrepareEvent()

def pyWAVECAT64CH_ReadEventBuffer():
    return wcDll.WAVECAT64CH_ReadEventBuffer()

def pyWAVECAT64CH_AllocateEventStructure():
    return wcDll.WAVECAT64CH_AllocateEventStructure(byref(theEvent))

def pyWAVECAT64CH_DecodeEvent():
    return wcDll.WAVECAT64CH_DecodeEvent(byref(theEvent))  
#WAVECAT64CH_DLL_API  WAVECAT64CH_ErrCode WAVECAT64CH_DecodeEvent(WAVECAT64CH_EventStruct *eventStruct);

def pyWAVECAT64CH_StopRun():
    return wcDll.WAVECAT64CH_StopRun()

def pyWAVECAT64CH_SetTriggerMode(triggerType):
    return wcDll.WAVECAT64CH_SetTriggerMode(c_uint(triggerType))

def pyWAVECAT64CH_GetTriggerMode():
    triggerType=c_uint(-1)
    errCode=wcDll.WAVECAT64CH_GetTriggerMode(byref(triggerType))
    return (errCode,triggerType)

def checkError(errCode):
    if (errCode!=0):
        print "Error!"
        print errCode
        os.exit(-1)


 