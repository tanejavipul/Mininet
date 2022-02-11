#!/usr/bin/python
import Image
import ImageDraw
import ImageFont
import random

def orientation(p, q, r):
	''' >0 if p-q-r are clockwise, <0 if counterclockwise, 0 if colinear. '''
	return (q[1]-p[1])*(r[0]-p[0]) - (q[0]-p[0])*(r[1]-p[1])

def flipPixel(pixel):
	if pixel == (0,0,0): return (255,255,255)
	else: return (0,0,0)


def flipColors(im,l):
	''' flip the colours on the right side of l '''
	(width, height)=im.size
	#draw=ImageDraw.Draw(im)
	pix=im.load()
	for x in range(width):
		for y in range(height):
			if orientation(l[0], l[1], (x,y))>0:
				pix[x,y] = flipPixel(pix[x,y])

def twoColor(im, lines):
	if lines==[]:
		pass
	else:
		twoColor(im,lines[:-1])
		flipColors(im,lines[-1])
	im.show()

	
width=400
height=400
lines=[]
for i in range(8):
	lines.append([(random.randint(0,width), random.randint(0,height)), (random.randint(0,width), random.randint(0,height))])

im = Image.new ( "RGB", (width,height), "#000" )
draw = ImageDraw.Draw ( im )

for l in lines:
	draw.line(l)
im.show()

#for l in lines:
#	flipColors(im,l)

im = Image.new ( "RGB", (width,height), "#000" )
draw = ImageDraw.Draw ( im )
twoColor(im, lines)

# for p in points:
#	draw.point(p, "#fff")

im.show()
