#!/usr/bin/env python2
# -*- coding: utf-8 -*-

'''
This python script is used to generate the WorldAirports database. The right input file
can be obtained from http://openflights.org/data.html and its file format is being used
here. The WorldAiports.db is a binary file, which contains as follows:
4-bytes integer that tells us how many airports we have;
size * 436 bytes of aiports data. 428 byte-struct contains:
	128 bytes of name (string);
	128 bytes of city (string);
	128 bytes of country (string);
	8 bytes of iata code (string);
	8 bytes of icao code (string);
	
	Now, we have list of FIRs that the airport belongs to.
	There are maximum two FIRs.
	If there is no FIR (airport belongs to no FIR or belongs to only one FIR),
	the FIR icao is "ZZZZ".
	4 bytes of int which is 0 or 1 (oceanic or not)
	8 bytes of first FIR ICAO
	4 bytes of int which is 0 or 1 (oceanic or not)
	8 bytes of second FIR ICAO
	
	4 bytes of latitude (float);
	4 bytes of longitude (float);
	4 bytes of altitude (int).

'''

import sys
from struct import *

firs = []

class Point(object):
	def __init__(self, x, y):
		self.x = float(x)
		self.y = float(y)


class Fir(object):
	def __init__(self, icao, oceanic, textPos):
		self.icao = icao
		self.oceanic = oceanic
		self.textPos = textPos
		self.coords = []



class Airport(object):
	def __init__(self, name, city, country, iata, icao, latitude, longitude, altitude):
		self.name = name.lstrip("\"").rstrip("\"")
		self.city = city.lstrip("\"").rstrip("\"")
		self.country = country.lstrip("\"").rstrip("\"")
		self.iata = iata.lstrip("\"").rstrip("\"")
		self.icao = icao.lstrip("\"").rstrip("\"")
		self.latitude = float(latitude) # Y
		self.longitude = float(longitude) # X
		self.altitude = int(altitude)
		self.firs = findFir(self)

	def data(self):
		fir_oceanic_a = 0 if len(self.firs) == 0 else self.firs[0].oceanic
		fir_oceanic_b = 0 if len(self.firs) <= 1 else self.firs[1].oceanic
		fir_icao_a = 'ZZZZ' if len(self.firs) == 0 else self.firs[0].icao
		fir_icao_b = 'ZZZZ' if len(self.firs) <= 1 else self.firs[1].icao
		return pack('128s128s128s8s8si8si8sffi', self.name, self.city, self.country,\
			self.iata, self.icao, fir_oceanic_a, fir_icao_a, fir_oceanic_b, \
			fir_icao_b, self.latitude, self.longitude, self.altitude)

def airportInFir(airport, fir):
	if not isinstance(airport, Airport):
		sys.exit("Give me an Airport object!")
	
	result = False
	j = len(fir.coords) - 1
	for i in range(len(fir.coords)):
		# wtf is going on here
		if (fir.coords[i].y < airport.latitude and fir.coords[j].y >= airport.latitude or \
				fir.coords[j].y < airport.latitude and fir.coords[i].y >= airport.latitude):
			if fir.coords[i].x + (airport.latitude - fir.coords[i].y) / \
					(fir.coords[j].y - fir.coords[i].y) * \
					(fir.coords[j].x - fir.coords[i].x) < airport.longitude:
				result = not result
		j = i
		
	return result

def findFir(airport):
	firs_stack = []
	firs_result = []
	for f in firs:
		if f.oceanic == 1 or (f.textPos.x == 0.0 and f.textPos.y == 0.0):
			firs_stack.append(f)
			continue
		
		if airportInFir(airport, f):
			firs_result.append(f)
	
	for f in firs_stack:
		if airportInFir(airport, f):
			firs_result.append(f)
	
	if len(firs_result) == 0:
		print ("Could not find FIR for %s!" % airport.icao)
	
	return firs_result


if len(sys.argv) < 3:
	sys.exit('Usage: %s [file] [file]' % sys.argv[0])

try:
	dbin = open(sys.argv[1], 'r')
except IOError:
	sys.exit('Could not open file %s for reading!' % sys.argv[1])

try:
	firdb = open(sys.argv[2], 'rb')
except IOError:
	sys.exit('Could not open file %s for reading!' % sys.argv[2])

offset = 0
contents = firdb.read()
(size,) = unpack_from('i', contents, offset)

offset += 4
for i in range(size):
	(icao,oceanic,extern1x,extern1y,extern2x,extern2y,tpx,tpy,counting) = \
		unpack_from('8siffffffi', contents, offset)
	
	current_fir = Fir(icao, oceanic, Point(tpx, tpy))
	
	offset += calcsize('8siffffffi')
	
	for k in range(counting):
		(pointX, pointY) = unpack_from('ff', contents, offset)
		current_fir.coords.append(Point(pointX, pointY))
		offset += calcsize('ff')
	
	(counting2,) = unpack_from('i', contents, offset)
	offset += 4

	for k in range(counting2):
		(fooIndex1,fooIndex2,fooIndex3) = unpack_from('HHH', contents, offset)
		offset += calcsize('HHH')
	
	firs.append(current_fir)

firdb.close()


dbout = open('WorldAirports.db', 'wb')
dbout.seek(4)

i = 0

for line in dbin:
	line = line.strip()
	apdata = line.split(',')

	# exclude non-ICAO airports
	if (apdata[5] == "\N") or (apdata[5] == "\"\""):
		continue
	
	ap = Airport(apdata[1], apdata[2], apdata[3], apdata[4], apdata[5], apdata[6], apdata[7], apdata[8])
	dbout.write(ap.data())

	i += 1

dbout.seek(0)
dbout.write(pack('i', i))

print 'Airports read: ' + str(i),

dbout.close()
dbin.close()


