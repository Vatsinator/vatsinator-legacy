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

#include "VatsinatorApplication.h"

#include "glutils/VertexBufferObject.h"
#include "glutils/glExtensions.h"

#include "vdebug/glErrors.h"

#include "WorldMap.h"
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
	for (Polygon& p: __polygons) {
		if (p.vbo.border)
			delete p.vbo.border;
		
		if (p.vbo.triangles)
			delete p.vbo.triangles;
	}
#endif
}

void
WorldMap::init() {
#ifdef VATSINATOR_PLATFORM_LINUX
#ifndef NO_DEBUG
	qDebug() << "Preparing VBOs for WorldMap...";
#endif
	for (Polygon& p: __polygons) {
		if (p.borders.isEmpty()) {
			p.vbo.border = NULL;
			p.vbo.triangles = NULL;
			continue;
		}
		
		p.vbo.border = new VertexBufferObject(GL_ARRAY_BUFFER);
		p.vbo.border->sendData(sizeof(Point) * p.borders.size(), &p.borders[0].x);
		
		p.vbo.borderSize = p.borders.size();
		p.borders.clear();
		
		if (p.triangles.isEmpty()) {
			p.vbo.triangles = NULL;
			continue;
		}
		
		p.vbo.triangles = new VertexBufferObject(GL_ELEMENT_ARRAY_BUFFER);
		p.vbo.triangles->sendData(sizeof(unsigned short) * p.triangles.size(), &p.triangles[0]);
		
		p.vbo.trianglesSize = p.triangles.size();
		p.triangles.clear();
	}
#ifndef NO_DEBUG
	qDebug() << "WorldMap VBOs ready.";
#endif
#endif
}

void
WorldMap::draw() const {
#ifdef VATSINATOR_PLATFORM_LINUX
	for (const Polygon& polygon: __polygons) {
		if (polygon.vbo.triangles && polygon.vbo.border) {
			polygon.vbo.border->bind();
			polygon.vbo.triangles->bind();
			
			glVertexPointer(2, GL_FLOAT, 0, 0); checkGLErrors(HERE);
			glDrawElements(GL_TRIANGLES, polygon.vbo.trianglesSize, GL_UNSIGNED_SHORT, 0); checkGLErrors(HERE);
			
			polygon.vbo.triangles->unbind();
			polygon.vbo.border->unbind();
		}
	}
#else
	for (const Polygon& p: __polygons) {
		if (!p.borders.isEmpty()) {
			glVertexPointer(2, GL_FLOAT, 0, &p.borders[0].x); checkGLErrors(HERE);
			glDrawElements(GL_TRIANGLES, p.triangles.size(), GL_UNSIGNED_SHORT, &p.triangles[0]); checkGLErrors(HERE);
		}
	}
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
	
	__polygons.resize(size);
	unsigned allTogether = 0;
	for (int i = 0; i < size; ++i) {
		int counting;
		db.read(reinterpret_cast< char* >(&counting), 4);
		//qDebug() << "First counting: " << counting;
		if (counting) {
			__polygons[i].borders.resize(counting);
			db.read(reinterpret_cast< char* >(&__polygons[i].borders[0].x), sizeof(Point) * counting);
			allTogether += counting;
		}
		
		db.read(reinterpret_cast< char* >(&counting), 4);
		//qDebug() << "Second counting: " << counting;
		if (counting) {
			__polygons[i].triangles.resize(counting);
			db.read(reinterpret_cast< char* >(&__polygons[i].triangles[0]), sizeof(unsigned short) * counting);
		}
		
	}
	
#ifndef NO_DEBUG
	qDebug() << "WorldMap coords: " << allTogether;
#endif
	
	db.close();
	
	/*
	 * Now we put some small polygons into one VBO in order not to create
	 * so many of them. This value is ok, as bigger would cause segfaults
	 * (bug on some ATI cards).
	 */
	int i = 0, offset = 0;
	Polygon temp;
	while (i < __polygons.size()) {
		if (__polygons[i].triangles.size() < 512) {
			for (const Point& p: __polygons[i].borders)
				temp.borders.push_back(p);
			for (const unsigned short c: __polygons[i].triangles)
				temp.triangles.push_back(c + offset);
			offset += __polygons[i].borders.size();
			__polygons.remove(i);
		} else {
			i += 1;
		}
	}
	__polygons.push_back(temp);
}


