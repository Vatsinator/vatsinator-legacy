/*
    worldmap.h
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


#ifndef WORLDMAP_H
#define WORLDMAP_H

#include <QVector>

#include "db/point.h"

#include "singleton.h"

class VertexBufferObject;

class WorldMap : public Singleton< WorldMap > {
	
	struct WorldMapVBO {
		VertexBufferObject *	border;
		int			borderSize;
		VertexBufferObject *	triangles;
		int			trianglesSize;
	};
	
	struct Polygon {
		QVector< Point >		borders;
		
		QVector< unsigned short >	triangles;
		
		WorldMapVBO			vbo;
	};
	
public:
	WorldMap();
	virtual ~WorldMap();
	
	void init();

	void draw() const;
	
private:
	void __readDatabase();
	
	Polygon __worldPolygon;
	
	
};

#endif // WORLDMAP_H
