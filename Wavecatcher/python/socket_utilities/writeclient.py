#small client based on the matlab function writeclient
__author__ = 'delerue'

import time
from datetime import datetime

# Echo client program
def writeclient(HOST,PORT,msg,verbose=1,waittime=0,logfile='',datedlog=''):
    "Writes msg on host HOST port PORT; verbose if verbose=1"
    import socket
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    if (verbose==1):
        print 'Sending message ', msg, ' to host ', HOST, ' on port ', PORT
    if (datedlog!=''):
        date_str=datetime.now().strftime("_%Y%m%d")
        logfile=datedlog+date_str+".txt"
    if (logfile!=''):
        f = open(logfile, 'a')
        time_str=datetime.now().strftime("%Y%m%d %H:%M:%S")
        f.write("("+time_str+") > "+msg+"\n")
    s.sendall(msg)
    time.sleep(waittime)
    data = s.recv(1024)
    s.close()
    if (logfile!=''):
        time_str=datetime.now().strftime("%Y%m%d %H:%M:%S")
        f.write("("+time_str+") < "+data+"\n")
        f.close()
    if (verbose==1):
        print 'Received', repr(data)
    return data

portValue=20020
print str(portValue)
writeclient('127.0.0.1',portValue,'get trigger',1)
time.sleep(0.05)
writeclient('127.0.0.1',portValue,'set trigger 2',1)
time.sleep(0.05)
writeclient('127.0.0.1',portValue,'get trigger',1)
time.sleep(0.05)
writeclient('127.0.0.1',portValue,'set trigger external',1)
time.sleep(0.05)
writeclient('127.0.0.1',portValue,'get trigger',1)
time.sleep(0.05)
writeclient('127.0.0.1',portValue,'set trigger soft',1)
time.sleep(0.05)
writeclient('127.0.0.1',portValue,'get trigger',1)
time.sleep(0.05)
writeclient('127.0.0.1',portValue,'set trigger normal',1)
time.sleep(0.05)
writeclient('127.0.0.1',portValue,'Hello',verbose=1)
time.sleep(0.05)
writeclient('127.0.0.1',portValue,'data',1)
time.sleep(0.05)
writeclient('127.0.0.1',portValue,'stop',1)
time.sleep(0.05)
