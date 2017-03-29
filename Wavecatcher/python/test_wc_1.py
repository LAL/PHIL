# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import ctypes

MAX_NUM_OF_DEVICES=8
MAX_DESCRIPTOR_SIZE=1024
SERNUM_SIZE=8
#define  WAVECAT64CH_MAX_DATA_SIZE 1024
#define  WAVECAT64CH_INFO_NOT_AVAILABLE -1
#define  WAVECAT64CH_NB_OF_CHANNELS_IN_SAMBLOCK 2
#define  WAVECAT64CH_MAX_NB_OF_FE_BOARDS 4
#define  WAVECAT64CH_NB_OF_CHANNELS_IN_SAMBLOCK 2
#define  WAVECAT64CH_MAX_TOTAL_NB_OF_SAMBLOCKS 36
#define  WAVECAT64CH_MAX_TOTAL_NB_OF_CHANNELS  72
#define  WAVECAT64CH_ADCTOVOLTS 0.00061

# Load DLL into memory.

wcDll = ctypes.CDLL ("C:\\work_delerue\\PHIL\\Wavecatcher\\python\\WaveCat64Ch.dll")

nDevices=ctypes.c_int(-1)
deviceDescriptor=(ctypes.c_char*MAX_DESCRIPTOR_SIZE*MAX_NUM_OF_DEVICES)()
c2=(ctypes.c_char*MAX_NUM_OF_DEVICES*SERNUM_SIZE)()
print(nDevices.value)
print(deviceDescriptor)
print(deviceSerNumChar)
wcDll.WAVECAT64CH_GetWaveCatDevices(ctypes.byref(deviceDescriptor),ctypes.byref(c2),ctypes.byref(nDevices))
print(nDevices.value)
print(deviceDescriptor)
print(deviceSerNumChar)
