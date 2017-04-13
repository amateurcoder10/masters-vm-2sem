#!/usr/bin/env python3

from serial import Serial
import bitarray
import time
import sys; print(sys.version)
print('\nAutomatic baud rate checker:Receiving end')

serialPort = Serial("/dev/ttyUSB0", timeout=5)#timeout after 5 seconds
print('ready to begin')
serialPort.stopbits=1;#1 stop bit
#serialPort.rtscts=True

if (serialPort.isOpen() == False):
    serialPort.open()

outStr = ''
inStr = ''

serialPort.flushInput()
serialPort.flushOutput()



ba=bitarray.bitarray()

serialPort.setRTS(1)
serialPort.flushOutput()

ct=serialPort.getCTS()
serialPort.flushInput()
i=1
while(i<256):
	baud=150*i
	serialPort.baudrate=baud
	time.sleep(1)
	inStr = serialPort.read(1)
	print('\nScanning at ',baud)
	try:
		inStr=inStr.decode()
	except:
		pass

	#print (inStr)
	i=i*2
	
	#serialPort.flushInput()
	if(inStr=='U'):
		print('Charcter matched! Baud rate found to be',baud)
		break
	


serialPort.close()
