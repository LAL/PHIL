#simple http request
__author__ = 'delerue'

from writeclient import *

URL_IP="10.0.1.112"

#writeclient(URL_IP,80,"GET /scrdata.htm?lang=en"+chr(10),1,waittime=0.1)
writeclient(URL_IP,5025,"*IDN?"+chr(10),1,waittime=0)
#writeclient(URL_IP,80,"GET /"+chr(13)+chr(10),1)
#writeclient(URL_IP,80,"GET /"+chr(10)+chr(13),1)
#writeclient(URL_IP,80,"GET /"+chr(13),1)


