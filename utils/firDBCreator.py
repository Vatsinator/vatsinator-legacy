#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
from struct import *
from math import sqrt

class Point(object):
	def __init__(self, x, y):
		self.x = x
		self.y = y


class Vector(object):
	def __init__(self, x, y):
		self.x = x
		self.y = y
	
	def length(self):
		return sqrt(self.x * self.x + self.y * self.y)


class Triangle(object):
	def __init__(self, a, b, c):
		if not isinstance(a, Point) or not isinstance(b, Point) or not isinstance(c, Point):
			raise TypeError('The argument must be an instance of class Point!')
		
		self.a = a
		self.b = b
		self.c = c
		
	def pointInRange(self, point):
		if not isinstance(point, Point):
			raise TypeError('The argument must be an instance of class Point!')
		
		# check if triangle
		if cosOfAngle(Vector(self.b.x - self.a.x, self.b.y - self.a.y), \
			Vector(self.c.x - self.a.x, self.c.y - self.a.y)) == 1:
				return False
		
		v0 = Vector(self.c.x - self.a.x, self.c.y - self.a.y)
		v1 = Vector(self.b.x - self.a.x, self.b.y - self.a.y)
		v2 = Vector(point.x - self.a.x, point.y - self.a.y)
		
		dot00 = dotProduct(v0, v0)
		dot01 = dotProduct(v0, v1)
		dot02 = dotProduct(v0, v2)
		dot11 = dotProduct(v1, v1)
		dot12 = dotProduct(v1, v2)
		
		try:
			invDenom = 1 / (dot00 * dot11 - dot01 * dot01)
		except ZeroDivisionError:
			print ('Please check: point: (' + str(point.x) + ', ' + str(point.y) + '), triangle: ((' + \
				str(self.a.x) + ', ' + str(self.a.y) + '), (' + \
				str(self.b.x) + ', ' + str(self.b.y) + '), (' + \
				str(self.c.x) + ', ' + str(self.c.y) + '))')
			#sys.exit("Division by zero; cannot continue.")
			invDenom = 0
			
			
		u = (dot11 * dot02 - dot01 * dot12) * invDenom
		v = (dot00 * dot12 - dot01 * dot02) * invDenom
		
		return u >= 0 and v >= 0 and u + v < 1
		
		
def dotProduct(v1, v2):
	if not isinstance(v1, Vector) or not isinstance(v2, Vector):
		raise TypeError('The argument must be an instance of class Vector!')
	
	return (v1.x * v2.x) + (v1.y * v2.y)


def pointInPolygon(coords, point):
	if not isinstance(point, Point):
		raise TypeError('The argument must be an instance of class Point!')
	for c in coords:
		if not isinstance(c, Point):
			raise TypeError('The argument must be a list of instances of class Point!')
	
	result = False
	j = len(coords) - 1
	
	for i in range(len(coords)):
		if (coords[i].y < point.y and coords[j].y >= point.y or \
				coords[j].y < point.y and coords[i].y >= point.y):
			if coords[i].x + (point.y - coords[i].y) / \
					(coords[j].y - coords[i].y) *  \
					(coords[j].x - coords[i].x) < point.x:
				result = not result
		j = i
	
	return result


def cosOfAngle(a, b):
	if not isinstance(a, Vector) or not isinstance(b, Vector):
		raise TypeError('The argument must be an instance of class Vector!')
	
	return dotProduct(a, b)/(a.length() * b.length())


def triangulate(coords):
	#print ('Triangulating...')
	i = 0
	triangles = []
	
	if coords[0].x == coords[-1].x and coords[0].y == coords[-1].y:
		del coords[-1]
	
	# remove duplicates
	c = 0
	while c < len(coords) - 1:
		if coords[c].x == coords[c + 1].x and coords[c].y == coords[c + 1].y:
			del coords[c+1]
			continue
		c += 1
	
	while len(coords) >= 3:
		if i >= len(coords):
			i = 0
		a = i +1
		if a >= len(coords):
			a = 0
		b = a + 1
		if b >= len(coords):
			b = 0
		
		temptriangle = Triangle(coords[i], coords[a], coords[b])
		isok = True
		for c in (coords[:i] + coords[i+3:]):
			if temptriangle.pointInRange(c):
				isok = False
				break
		
		center = Point((coords[i].x + coords[b].x) / 2, (coords[i].y + coords[b].y) / 2)
		if len(coords) > 3 and not pointInPolygon(coords, center):
			isok = False
		
		if not isok:
			i += 1
			continue
		
		triangles.append(temptriangle)
		del coords[a]
	
	return triangles


if len(sys.argv) <2:
	sys.exit('Usage: %s [file]' % sys.argv[0])

try:
	dbin = open(sys.argv[1], 'r')
except IOError:
	sys.exit('Could not open file %s for reading!' % sys.argv[1])

dbout = open('WorldFirs.db', 'wb')
dbout.seek(4)

print ('Generating database...')

i = 0
counting = 0
firParsing = False
coords = []

for line in dbin:
	line = line.strip()
	firData = line.split(' ')
	
	if not firParsing:
		counting = int(firData[3])
		firParsing = True
		oceanic = int(0)
		
		icao = firData[0]
		if firData[1] == '1':
			oceanic = int(1)

		textX = float(firData[9])
		textY = float(firData[8])

		if firData[2] == '1':
			textX = 0.0
			textY = 0.0
		
		dbout.write(pack('4siddddddi', icao, oceanic, float(firData[5]), float(firData[4]),\
			float(firData[7]), float(firData[6]), textX, textY,\
			counting))
		
		print ('Processing ' + icao + '...')
	else:
		dbout.write(pack('dd', float(firData[1]), float(firData[0])))
		counting -= 1
		
		coords.append(Point(float(firData[1]), float(firData[0])))
		
		if counting == 0:
			firParsing = False
			i += 1
			
			triangulated = triangulate(coords)
			del coords[:]
			
			dbout.write(pack('i', len(triangulated)))
			for t in triangulated:
				dbout.write(pack('dddddd', t.a.x, t.a.y, t.b.x, t.b.y, t.c.x, t.c.y))
			

dbout.seek(0)
dbout.write(pack('i', i))

print('Firs read: ' + str(i))


dbout.close()
dbin.close()

