
#!/usr/bin/env python3

from serial import Serial
import bitarray
import time
import bitarray
import sys; print(sys.version)
print('\nDemonstrating M2M communication using RS232\n')

serialPort = Serial("/dev/ttyUSB0", timeout=5)#timeout after 5 seconds
#serialPort.baudrate=input("\nEnter Baud rate: ")#baud rate

#serialPort.stopbits=1;#1 stop bit
#serialPort.rtscts=True
#serialPort.dtrdsr=True
if (serialPort.isOpen() == False):
    serialPort.open()

	
ba=bitarray.bitarray()	

j=0
k=1
"""
while(j<9):
	j+=1
	k=k^1
	#print(k)
	serialPort.setRTS(k)

"""
i=0;
li=[]
#ba=[]

#print(serialPort.getCTS())

#time.sleep(1)
#serialPort.setDTR(1)
while(serialPort.getDSR()==0):
	#print(he)
	pass
#while serialPort.getCTS()==1:
#	pass

#print('dsr high going to receive')
#if(serialPort.getDSR()):
for i in range (0,8):
			#print('a',serialPort.getDSR())
			#start=time.time()
		
			#while(serialPort.getDSR()==0):
			#	pass
		
			#serialPort.setDTR(0)
		ba.append(serialPort.getCTS())
		time.sleep(1)
			#time.sleep(0.003)
			#print('a',serialPort.getDSR())
	
			#time.sleep(0.09)
			#end=time.time()
			#duration=end-start
			#print(duration)
			#serialPort.setDTR(1)
			#print(ct)
	
	
print(ba)	
print (ba.tostring())
	
	

serialPort.close()
