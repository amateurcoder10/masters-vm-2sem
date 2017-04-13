
from serial import Serial
import time
import cv2
import numpy as np

#ima = cv2.imread("img.png",0)
height, width= 11,11
img = np.zeros((height, width), np.uint8)

#10 white line of one pixel width
for i in range(height):
	for j in range(width):
		if(i%5==0):
			img[i][j]=255
		if(j%5==0):
			img[i][j]=255

cv2.imwrite("TX_Image.jpg",img)
imag= np.array(img,dtype = int )
#print imag.dtype
rows,cols = imag.shape

print ("Image size is",rows,"*",cols)
print("Transmitted a image")
print (imag)

image=imag.flatten()

#print image.shape
#print len(image)

serialPort = Serial("/dev/ttyUSB1", timeout=150)
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
"""
	out=str(image[row])
	if(len(out)== 2):
		ou= '0'+out	
	elif len(out)==1 :
		o1= '0'+out
		ou= '0'+o1
	else:
		ou=out
	if(len(ou)!=3):
		print defectives
"""
i=0
for row in range(0,len(image)):
	out= "%3d" %image[row]
	#out = str(image[row])
	#print "output",i, out
	outstr=out.encode()
	ret=serialPort.write(outstr)
	i=i+1
#	print image[row]
img_RX= np.zeros((rows, cols),np.int8)
#print img_RX
#print image.size
#print ret
    #time.sleep(0.05)
i=0
ct=serialPort.getCTS()
if serialPort.getCTS():	
	for row in range(0,rows):
		for col in range(0,cols):
			inn=serialPort.read(3)
			instr=inn.decode()
			#print i,instr
			img_RX[row][col]=int(instr)
			#print "image input",img_RX[row][col]
			i=i+1
			
#print("out of loop")
print("Received a Image")		
img=np.uint8(img_RX)
print (img)
#print ima
#print img.dtype
#print img.shape
#print img.size

cv2.imwrite("RX_Image.jpg",img)
#print "done"

serialPort.close()

