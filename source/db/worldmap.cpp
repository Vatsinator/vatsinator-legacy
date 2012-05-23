/*
    WorldMap.cpp
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <fstream>

#include <QtGui>

#include "vatsinatorapplication.h"

#include "glutils/vertexbufferobject.h"
#include "glutils/glextensions.h"

#include "vdebug/glerrors.h"

#include "worldmap.h"
#include "defines.h"


using namespace std;

WorldMap::WorldMap() {
#ifndef NO_DEBUG
	std::cout << "WORLD_MAP: " << WORLD_MAP << std::endl;
#endif
	__readDatabase();
	
}

WorldMap::~WorldMap() {
#ifdef VATSINATOR_PLATFORM_LINUX
	delete __worldPolygon.vbo.border;
	delete __worldPolygon.vbo.triangles;
#endif
}

void
WorldMap::init() {
#ifdef VATSINATOR_PLATFORM_LINUX
#ifndef NO_DEBUG
	qDebug() << "Preparing VBOs for WorldMap...";
#endif
	
	__worldPolygon.vbo.border = new VertexBufferObject(GL_ARRAY_BUFFER);
	__worldPolygon.vbo.border->sendData(sizeof(Point) * __worldPolygon.borders.size(),
					    &__worldPolygon.borders[0].x);
	
	__worldPolygon.vbo.borderSize = __worldPolygon.borders.size();
	__worldPolygon.borders.clear();
	
	__worldPolygon.vbo.triangles = new VertexBufferObject(GL_ELEMENT_ARRAY_BUFFER);
	__worldPolygon.vbo.triangles->sendData(sizeof(unsigned short) * __worldPolygon.triangles.size(),
					       &__worldPolygon.triangles[0]);
	
	__worldPolygon.vbo.trianglesSize = __worldPolygon.triangles.size();
	__worldPolygon.triangles.clear();
	
#ifndef NO_DEBUG
	qDebug() << "WorldMap VBOs ready.";
#endif
#endif
}

void
WorldMap::draw() const {
#ifdef VATSINATOR_PLATFORM_LINUX
	__worldPolygon.vbo.border->bind();
	__worldPolygon.vbo.triangles->bind();
	
	glVertexPointer(2, GL_FLOAT, 0, 0); checkGLErrors(HERE);
	glDrawElements(GL_TRIANGLES, __worldPolygon.vbo.trianglesSize, GL_UNSIGNED_SHORT, 0); checkGLErrors(HERE);
	
	__worldPolygon.vbo.triangles->unbind();
	__worldPolygon.vbo.border->unbind();
#else
	glVertexPointer(2, GL_FLOAT, 0, &__worldPolygon.borders[0].x); checkGLErrors(HERE);
	glDrawElements(GL_TRIANGLES, __worldPolygon.triangles.size(), GL_UNSIGNED_SHORT,
		       &__worldPolygon.triangles[0]); checkGLErrors(HERE);
#endif
}

void WorldMap::__readDatabase() {
	fstream db(WORLD_MAP, ios::in | ios::binary);
	
	int size;
	db.read(reinterpret_cast< char* >(&size), 4);
	db.seekg(4);
	
#ifndef NO_DEBUG
	qDebug() << "WorldMap polygons: " << size;
#endif
	
	QVector< Polygon > polygons;
	
	polygons.resize(size);
	unsigned allTogether = 0;
	for (int i = 0; i < size; ++i) {
		int counting;
		db.read(reinterpret_cast< char* >(&counting), 4);
		//qDebug() << "First counting: " << counting;
		if (counting) {
			polygons[i].borders.resize(counting);
			db.read(reinterpret_cast< char* >(&polygons[i].borders[0].x), sizeof(Point) * counting);
			allTogether += counting;
		}
		
		db.read(reinterpret_cast< char* >(&counting), 4);
		//qDebug() << "Second counting: " << counting;
		if (counting) {
			polygons[i].triangles.resize(counting);
			db.read(reinterpret_cast< char* >(&polygons[i].triangles[0]), sizeof(unsigned short) * counting);
		}
		
	}
	
#ifndef NO_DEBUG
	qDebug() << "WorldMap coords: " << allTogether;
#endif
	
	db.close();
	
	/* Move all the polygons to one polygon */
	int offset = 0;
	for (Polygon& p: polygons) {
		for (const Point& pt: p.borders)
			__worldPolygon.borders.push_back(pt);
		for (const unsigned short c: p.triangles)
			__worldPolygon.triangles.push_back(c + offset);
		offset += p.borders.size();
	}
	
}


