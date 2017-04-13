

from serial import Serial
import time
import cv2
import numpy as np
import os

activate_this = '/path/to/env/bin/activate_this.py'
execfile(activate_this, dict(__file__=activate_this))

image = cv2.imread("abc.png",0)
#image= np.array(ima,dtype = )
print (image.dtype)
rows,cols = image.shape

print (rows)
print (cols)
print (image)

image.flatten()
print (len(image))

serialPort = Serial("/dev/ttyUSB3", timeout=10)
serialPort.baudrate=input("Enter Baud rate: ")
serialPort.rtscts=True
if (serialPort.isOpen() == False):
    serialPort.open()

outStr = ''
inStr = ''

serialPort.flushInput()
serialPort.flushOutput()

#for i, a in enumerate(range(33, 126)):
# outStr += chr(a)
#outStr=input("Enter data to be transmitted: ")
#outStr=outStr.encode()
serialPort.setRTS(1)

for row in range(0,rows):
	for col in range(0,cols):
		serialPort.write(image[row][col])

img_RX= np.zeros((rows, cols), dytpe=int)
print (img_RX)
print (image.size)
    #time.sleep(0.05)
ct=serialPort.getCTS()
if serialPort.getCTS():	
	for row in range(0,rows):
		for col in range(0,cols):
			img_RX[row][col]=serialPort.read(image.size)

img=np.uint8(img_RX)
cv2.imwrite("RX_tornado.jpg",img)
#inStr=inStr.decode()
#ba.fromstring(inStr)
#print(ba)
"""
print("Serial Port Input is : ",inStr)
outStr=outStr.decode()
#print("outStr = " + outStr)
if(inStr == outStr):
        print ("Loop back Test WORKED! for length of ",len(outStr))
else:
        print("failed")
"""
serialPort.close()
