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
     
PORT = 20040
HOST=''

#Initiates the connexion with the wavecatcher 
nDevices, deviceDescriptor, deviceSerNumChar = pyWAVECAT64CH_GetWaveCatDevices()

if nDevices.value!=1:
    print "Incorrect number of devices: "+str(nDevices.value)+" device(s) detected." 
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
dataReady=True

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
    print 'Received data[x:y]: >', dataFromSocket[0:9],"<"
    
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
    elif dataFromSocket.lower()=="get frequency":
        (errCode,frequency)=pyWAVECAT64CH_GetSamplingFrequency()
        print "Frequency"
        print repr(frequency.value)
        checkError(errCode)
        replyToSocket="Frequency: "+str(frequency.value)
    elif dataFromSocket[0:13].lower()=="set frequency":
        if len(dataFromSocket)>14:
            frequency=int(dataFromSocket[14:].strip())
            print "frequency "+dataFromSocket[14:]
            print "frequency "+str(frequency)
            errCode = pyWAVECAT64CH_SetSamplingFrequency(frequency)
            if (errCode==0):
                replyToSocket="Frequency changed to "+str(frequency)
            elif errCode==-16:
                replyToSocket="Incorrect frequency specified"
                print replyToSocket                     
                errCode=0
        else:
            replyToSocket="Incorrect frequency specified"
            print replyToSocket     
        checkError(errCode)
    elif (dataFromSocket[0:21].lower()=="set threshold channel")or(dataFromSocket[0:21].lower()=="get threshold channel"):
        if (len(dataFromSocket)>24):
            endChannel=24
        else:
            endChannel=len(dataFromSocket)      
        channel=int(dataFromSocket[22:endChannel].strip())
        print "channel ",str(channel)
        if dataFromSocket[0:21].lower()=="set threshold channel":
            value=float(dataFromSocket[endChannel+5:])
            print "Value "+str(value)
            errCode=pyWAVECAT64CH_SetTriggerThreshold(channel,value)
            replyToSocket="Threshold set on channel "+ str(channel)
            checkError(errCode)
        elif dataFromSocket[0:21].lower()=="get threshold channel":
            (errCode,threshold)=pyWAVECAT64CH_GetTriggerThreshold(channel)
            replyToSocket="Threshold on channel "+str(channel)+": "+str(threshold)
            print replyToSocket 
            checkError(errCode)
    elif dataFromSocket[0:9].lower()=="set delay":
        if len(dataFromSocket)>10:
            delay=int(dataFromSocket[10:].strip())
            print "delay "+str(delay)
            errCode = pyWAVECAT64CH_SetTriggerDelay(delay)
            if (errCode==0):
                replyToSocket="Delay changed to "+str(delay)
            elif errCode==-16:
                replyToSocket="Incorrect delay specified"
                print replyToSocket                     
                errCode=0
        else:
            replyToSocket="Incorrect delay specified"
            print replyToSocket     
        checkError(errCode)
    elif dataFromSocket.lower()=="get delay":
        (errCode,delay)=pyWAVECAT64CH_GetTriggerDelay()
        checkError(errCode)
        replyToSocket="Delay: "+str(delay.value)
    elif dataFromSocket[0:11].lower()=="set channel":
        if len(dataFromSocket)>=14:
            channel=int(dataFromSocket[12:13].strip())
            state=int(dataFromSocket[14:].strip())
            stateBol=(state==1)
            print "channel "+str(channel)+" "+str(stateBol)
            errCode = pyWAVECAT64CH_SetChannelState(channel,stateBol)
            if (errCode==0):
                replyToSocket="State on channel "+str(channel)+" changed to "+str(state)
            else:
                replyToSocket="Channel state change failed"
            print replyToSocket                     
        else:
            replyToSocket="Incorrect channel specified"
            print replyToSocket     
        checkError(errCode)
    elif dataFromSocket[0:11].lower()=="get channel":
        if len(dataFromSocket)>12:
            channel=int(dataFromSocket[12:13].strip())
            (errCode,state)= pyWAVECAT64CH_GetChannelState(channel)
            if (errCode==0):
                replyToSocket="State on channel "+str(channel)+" is "+str(state)
            else:
                replyToSocket="Channel status check failed"
            print replyToSocket                     
        else:
            replyToSocket="Incorrect channel specified"
            print replyToSocket     
        checkError(errCode)
    elif dataFromSocket.lower()=="check data":
        print "Sending data:"       
        print "Prepare..."       
        errCode += pyWAVECAT64CH_PrepareEvent()
        checkError(errCode)
        print "Read..."       
        errCode += pyWAVECAT64CH_ReadEventBuffer()
        if errCode==0:
            replyToSocket="Data ready"
            dataReady=True
            print "Decode..."       
            errCode += pyWAVECAT64CH_DecodeEvent()
            print repr(theEvent._fields_)
            print repr(theEvent.ChannelData)
            print repr(theEvent.ChannelData[0].WaveformDataSize)
            print repr(theEvent.ChannelData[0].WaveformData)
        elif errCode==7:            
            replyToSocket="No new data"
            errCode=0
            dataReady=False
        else:
            replyToSocket="Error..."
            dataReady=False
    elif dataFromSocket.lower()=="get event number":
        if dataReady==True:
            replyToSocket="Event "+str(theEvent.EventID)
            errCode=0            
    elif (dataFromSocket[0:9].lower()=="integrate")or(dataFromSocket[0:9].lower()=="substract"):
        if dataReady==True:
            tokens=dataFromSocket.split()
            print tokens
            if (tokens[1]=="channel")and(tokens[3]=="from")and(tokens[5]=="to"):
                channel=int(tokens[2])
                fromValue=int(tokens[4])
                toValue=int(tokens[6])
                print  "Integrate from "+str(channel)+" from "+str(fromValue)+" to "+str(toValue)
                integral=0
                for idata in range(fromValue,toValue):
                    integral+=theEvent.ChannelData[channel].WaveformData[idata]
                integral=integral/(toValue-fromValue)
                print len(tokens)
                if (len(tokens)==11)and(tokens[7]=="minus")and(tokens[9]=="to"):
                    fromValue=int(tokens[8])
                    toValue=int(tokens[10])
                    print  "Substract from "+str(channel)+" from "+str(fromValue)+" to "+str(toValue)
                    subval=0
                    for idata in range(fromValue,toValue):
                        subval+=theEvent.ChannelData[channel].WaveformData[idata]
                        subval=subval/(toValue-fromValue)
                    substraction=integral-subval
                    replyToSocket="Substraction "+str(substraction)                    
                else:
                    replyToSocket="Integral "+str(integral)
            errCode=0            
        else:
            replyToSocket="Data not ready!"
            errCode=0            
    elif dataFromSocket.lower()=="get all data":
        if dataReady==True:
            for ichan in range(1,7):
                txt=""
                for idata in range(0,theEvent.ChannelData[0].WaveformDataSize):
                    txt=txt+" "+str(theEvent.ChannelData[ichan].WaveformData[idata])
                print txt
            replyToSocket="Data:\n"+txt+"\nData end."
        else:
            replyToSocket="Data not ready!"
            print replyToSocket
            
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
print "Error code: "+str(errCode)


