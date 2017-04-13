#!/usr/bin/env python3

from serial import Serial
import bitarray
import time
import sys; print(sys.version)
print('\nDemonstrating M2M communication using RS232\n')
serialPort = Serial("/dev/ttyUSB0", timeout=5)#timeout after 5 seconds
serialPort.baudrate=input("\nEnter Baud rate: ")#baud rate
serialPort.stopbits=1;#1 stop bit
serialPort.rtscts=True
if (serialPort.isOpen() == False):
    serialPort.open()

outStr = ''
inStr = ''

serialPort.flushInput()
serialPort.flushOutput()


while(True):
	outStr=input("\nEnter your msg and -1 to quit: ")
	
	
	if(outStr=='-1'):
		#print (outStr)
		print('exiting')
		break
	outStr=outStr.encode()#encode the string
	


	serialPort.setRTS(1)
	serialPort.write(outStr)
	    
	ct=serialPort.getCTS()
	if serialPort.getCTS():
		inStr = serialPort.read(10) 
	

	ba=bitarray.bitarray()
	ba.frombytes(inStr)
	
	print("\n")
	inStr=inStr.decode()

	print('Received :' +inStr)
	outStr=outStr.decode()
	print("Sent : " + outStr)

serialPort.close()
