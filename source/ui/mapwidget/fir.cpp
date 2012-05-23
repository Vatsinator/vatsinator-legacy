/*
    Fir.cpp
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

#include <QtGui>
#include <qgl.h>
#include <GL/glext.h>

#include "glutils/vertexbufferobject.h"
#include "glutils/glextensions.h"

#include "ui/mapwidget/mapwidget.h"

#include "vdebug/glerrors.h"

#include "fir.h"

#include "defines.h"

Fir::Fir() {
}

Fir::~Fir() {
	MapWidget::deleteImage(icaoTip);
	
#ifdef VATSINATOR_PLATFORM_LINUX
	if (__trianglesVBO)
		delete __trianglesVBO;
	
	delete __bordersVBO;
#endif
}

void
Fir::addStaff(const Controller* _c) {
	__staff.push_back(_c);
}

void
Fir::addUirStaff(const Controller* _c) {
	__uirStaff.push_back(_c);
}

void
Fir::addFlight(const Pilot* _p) {
	__flights.push_back(_p);
}

void
Fir::addAirport(const AirportObject* _ap) {
	__airports.push_back(_ap);
}

void
Fir::correctName() {
	if (!name.endsWith("Radar") &&
			!name.endsWith("Control") &&
			!name.endsWith("Oceanic"))
		name += " Center";
}

void
Fir::init() {
	__generateTip();
	__prepareVBO();
}

void
Fir::drawBorders() const {
#ifdef VATSINATOR_PLATFORM_LINUX
	__bordersVBO->bind();
	
	glVertexPointer(2, GL_FLOAT, 0, 0); checkGLErrors(HERE);
	glDrawArrays(GL_LINE_LOOP, 0, __bordersSize); checkGLErrors(HERE);
	
	__bordersVBO->unbind();
#else
	glVertexPointer(2, GL_FLOAT, 0, &borders[0].x); checkGLErrors(HERE);
	glDrawArrays(GL_LINE_LOOP, 0, borders.size()); checkGLErrors(HERE);
#endif
}

void
Fir::drawTriangles() const {
#ifdef VATSINATOR_PLATFORM_LINUX
	if (__trianglesSize) {
		__bordersVBO->bind();
		__trianglesVBO->bind();
		
		glVertexPointer(2, GL_FLOAT, 0, 0); checkGLErrors(HERE);
		glDrawElements(GL_TRIANGLES, __trianglesSize, GL_UNSIGNED_SHORT, 0); checkGLErrors(HERE);
		
		__trianglesVBO->unbind();
		__bordersVBO->unbind();
	}
#else
	if (!triangles.isEmpty()) {
		glVertexPointer(2, GL_FLOAT, 0, &borders[0].x); checkGLErrors(HERE);
		glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_SHORT, &triangles[0]); checkGLErrors(HERE);
	}
#endif
}

void
Fir::__generateTip() {
	QString icao(header.icao);
	if (header.oceanic) {
		icao = icao.left(4) + " Oceanic";
	}
	
	icao = icao.simplified();
	
	if (header.textPosition.x == 0.0 && header.textPosition.y == 0.0) {
		icaoTip = 0;
		return;
	}
	
	QImage temp(MapWidget::GetSingleton().getFirToolTipBackground());
	QPainter painter(&temp);
	painter.setRenderHint(QPainter::TextAntialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);
	painter.setFont(MapWidget::GetSingleton().getFirFont());
	painter.setPen(QColor(FIRS_LABELS_FONT_COLOR));
	QRect rectangle(0, 0, 64, 24);
	painter.drawText(rectangle, Qt::AlignCenter | Qt::TextWordWrap, icao);
	icaoTip = MapWidget::loadImage(temp);
}

void
Fir::__prepareVBO() {
#ifdef VATSINATOR_PLATFORM_LINUX
	__bordersVBO = new VertexBufferObject(GL_ARRAY_BUFFER);
	__bordersVBO->sendData(sizeof(Point) * borders.size(), &borders[0].x);
	
	__bordersSize = borders.size();
	borders.clear();
	
	if (!triangles.isEmpty()) {
		__trianglesVBO = new VertexBufferObject(GL_ELEMENT_ARRAY_BUFFER);
		__trianglesVBO->sendData(sizeof(unsigned short) * triangles.size(), &triangles[0]);
		
		__trianglesSize = triangles.size();
		triangles.clear();
	} else
		__trianglesVBO = NULL;
#endif
}


