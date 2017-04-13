#!/usr/bin/env python3

from serial import Serial
import bitarray
import time
import sys; print(sys.version)

serialPort = Serial("/dev/ttyUSB3", timeout=5)#timeout after 5 seconds
serialPort.baudrate=input("Enter Baud rate: ")#baud rate
serialPort.stopbits=1;#1 stop bit
serialPort.rtscts=True
if (serialPort.isOpen() == False):
    serialPort.open()

outStr = ''
inStr = ''

serialPort.flushInput()
serialPort.flushOutput()

#for i, a in enumerate(range(33, 126)):
# outStr += chr(a)
outStr=input("Enter data to be transmitted: ")
ba=bitarray.bitarray()
ba.fromstring(outStr)#string to be sent out
print(ba)
print(len(ba))
outStr=outStr.encode()#encode the string


serialPort.setRTS(1)
serialPort.write(outStr)
    #time.sleep(0.05)
ct=serialPort.getCTS()
if serialPort.getCTS():
	inStr = serialPort.read(10) 
#print(ct)

ba=bitarray.bitarray()
ba.frombytes(inStr)
print(ba)
print(len(ba))
print("\n")
inStr=inStr.decode()

print('Serial Port Input is :' +inStr)
outStr=outStr.decode()
print("Serial port output is: " + outStr)
if(inStr == outStr):
        print ('Loop back Test WORKED for length of ',+len(outStr))
else:
        print('failed')

serialPort.close()
