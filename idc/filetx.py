#!/usr/bin/python3

from serial import Serial
import time
print('reading from a file and sending using RS232')
rfile=open("txdata","r")
wfile=open("rxdata","w")
serialPort = Serial("/dev/ttyUSB3", timeout=5)
serialPort.baudrate=input("Enter Baud rate: ")
serialPort.rtscts=True
if (serialPort.isOpen() == False):
    serialPort.open()

outStr = ''
inStr = ''

serialPort.flushInput()
serialPort.flushOutput()

time.sleep(1)
outStr=rfile.read()
outStr=outStr.encode()
serialPort.setRTS(1)
serialPort.write(outStr)
   
ct=serialPort.getCTS()
if serialPort.getCTS():
	inStr = serialPort.read(100) 

inStr=inStr.decode()

wfile.write(inStr)
print("Received: ",inStr)
outStr=outStr.decode()

serialPort.close()
rfile.close()
wfile.close()
