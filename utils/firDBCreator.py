#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
from struct import *


if len(sys.argv) <2:
	sys.exit('Usage: %s [file]' % sys.argv[0])

try:
	dbin = open(sys.argv[1], 'r')
except IOError:
	sys.exit('Could not open file %s for reading!' % sys.argv[1])

dbout = open('WorldFirs.db', 'wb')
dbout.seek(4)

i = 0
counting = 0
firParsing = False

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
	else:
		dbout.write(pack('dd', float(firData[1]), float(firData[0])))
		counting -= 1
		
		if counting == 0:
			firParsing = False
			i += 1

dbout.seek(0)
dbout.write(pack('i', i))

print 'Firs read: ' + str(i),


dbout.close()
dbin.close()

