#!/usr/bin/env python2
# -*- coding: utf-8 -*-

'''
This python script is used to generate the WorldAirports database. The right input file
can be obtained from http://openflights.org/data.html and its file format is being used
here. The WorldAiports.db is a binary file, which contains as follows:
4-bytes integer that tells us how many airports we have;
size * 420 bytes of aiports data. 420 byte-struct contains:
	128 bytes of name (string);
	128 bytes of city (string);
	128 bytes of country (string);
	8 bytes of iata code (string);
	8 bytes of icao code (string);
	8 bytes of latitude (double);
	8 bytes of longitude (double);
	4 bytes of altitude (int).

After the database is created it should be checked by dbCheck program.

'''

import sys
from struct import *

class Airport(object):
	def __init__(self, name, city, country, iata, icao, latitude, longitude, altitude):
		self.name = name.lstrip("\"").rstrip("\"")
		self.city = city.lstrip("\"").rstrip("\"")
		self.country = country.lstrip("\"").rstrip("\"")
		self.iata = iata.lstrip("\"").rstrip("\"")
		self.icao = icao.lstrip("\"").rstrip("\"")
		self.latitude = float(latitude)
		self.longitude = float(longitude)
		self.altitude = int(altitude)

	def data(self):
		return pack('128s128s128s8s8sddi', self.name, self.city, self.country,\
			self.iata, self.icao, self.latitude, self.longitude, self.altitude)

if len(sys.argv) <2:
	sys.exit('Usage: %s [file]' % sys.argv[0])

try:
	dbin = open(sys.argv[1], 'r')
except IOError:
	sys.exit('Could not open file %s for reading!' % sys.argv[1])

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


