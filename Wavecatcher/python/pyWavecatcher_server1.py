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
import socket
     
PORT = 20020
HOST=''

#Initiates the connexion with the wavecatcher 
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

dataTakingStarted=False

continueAcquiring=True
#infinte loop until the server tells to stop
while continueAcquiring==True:
    print 'Opening new connection on port '+ str(PORT)
    print ' '
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((HOST, PORT))
    s.listen(1)
    conn, addr = s.accept()
    print 'Connected by', addr
    dataFromSocket = conn.recv(1024)
    print 'Received data: ', dataFromSocket
    print 'data length: ',len(dataFromSocket)
    
    print "[0:11] ", dataFromSocket[0:11]
    
    replyToSocket="Unkonwn"
    
    if dataFromSocket.lower()=="start":
        errCode += pyWAVECAT64CH_StartRun()   
        checkError(errCode)
        time.sleep(1)
        dataTakingStarted=True
        replyToSocket="Data taking started"
    elif dataFromSocket.lower()=="get trigger":
        (errCode,triggerType)=pyWAVECAT64CH_GetTriggerMode()
        print "Trigger type"
        print repr(triggerType.value)
        checkError(errCode)
        replyToSocket="Trigger type: "+str(triggerType.value)
    elif dataFromSocket[0:11].lower()=="set trigger":
        print "Set trigger mode:"
        if (dataFromSocket[12:].strip().lower()=="soft"):
            triggerVal=0
        elif (dataFromSocket[12:].strip().lower()=="normal"):
            triggerVal=1
        elif (dataFromSocket[12:].strip().lower()=="internal"):
            triggerVal=2
        elif (dataFromSocket[12:].strip().lower()=="external"):
            triggerVal=3
        elif (dataFromSocket[12:].strip().lower()=="coincidence"):
            triggerVal=4
        elif (dataFromSocket[12:].strip().lower()=="paired"):
            triggerVal=5
        else:
            try:    
                triggerVal=int(dataFromSocket[12:])
            except:
                print "Unable to understand: "+ dataFromSocket[12:]
                triggerVal=1
        print str(triggerVal)
        replyToSocket="Trigger set to "+str(triggerVal)
        errCode +=pyWAVECAT64CH_SetTriggerMode(triggerVal)
        checkError(errCode)
    elif dataFromSocket.lower()=="data":
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
        replyToSocket="Data"
    elif dataFromSocket.lower()=="stop":
        continueAcquiring=False
        replyToSocket="bye"
    elif dataFromSocket.lower()=="quit":
        continueAcquiring=False
        replyToSocket="bye"
    else:
        print "Command ", dataFromSocket, " was not understood"        
        replyToSocket="Command not understood!"

#functions to add: pause          
    checkError(errCode)
    print "Reply:  ", replyToSocket
    conn.sendall(replyToSocket)
    conn.close()
		
checkError(errCode)
errCode = pyWAVECAT64CH_StopRun();
checkError(errCode)
errCode = pyWAVECAT64CH_CloseDevice();
checkError(errCode)

pyWAVECAT64CH_CloseDevice()
checkError(errCode)

print "end"
print errCode


