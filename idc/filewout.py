#!/usr/bin/env python3
# Final without rx and tx 
from serial import Serial
import bitarray
import time
import sys; print(sys.version)
import bitarray
print('\nDemonstrating M2M communication using RS232 without tx and rx\n')
rfile=open("txdata","r")
outStr=rfile.read()

ba = bitarray.bitarray()
ba.fromstring(outStr)


print(len(ba))
#print (ba.tostring())

serialPort = Serial("/dev/ttyUSB0")#timeout after 5 seconds

if (serialPort.isOpen() == False):
    serialPort.open()


serialPort.setDTR(0)

for ch in ba:    
    serialPort.setRTS(ch)
    time.sleep(1)
    
    

serialPort.setDTR(1)
time.sleep(0.5)    

serialPort.close()
