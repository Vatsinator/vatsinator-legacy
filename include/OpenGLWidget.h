/*
    OpenGLWidget.h
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


#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QGLWidget>
#include <QLabel>

#include "VatsimDataHandler.h"

class Clickable;
class VatsinatorApplication;
class MetarsWindow;
class FlightDetailsWindow;

enum PMMatrixMode {
	AIRPORTS_PILOTS, WORLD
};

class OpenGLWidget :
		public QGLWidget {
	
	/*
	 * We need this class to build our QGLWidget for UserInterface class.
	 */
	
	Q_OBJECT
	
public:
	OpenGLWidget(QWidget* = 0);
	
	virtual ~OpenGLWidget();
	
	void init();
	
	void paintGL();
	
	const Pilot *& getTrackedPilot() { return __tracked; }
	
public slots:
	void trackFlight(const Pilot* _p) { __tracked = _p; }
	
protected:
	/* Here we reimplement some functions that are needed for
	 * OpenGL to work properly. */
	void initializeGL();
	void resizeGL(int, int);
	void wheelEvent(QWheelEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void keyPressEvent(QKeyEvent*);
	
private:
	
	/**
	 * Prepares the proper matrix.
	 */
	void __prepareMatrix(PMMatrixMode);
	
	/**
	 * Draws all aiports on the map.
	 */
	void __drawAirports();
	
	/**
	 * Draws all pilots on the map.
	 */
	void __drawPilots();
	
	void __drawLines();
	
	void __openContextMenu(const Pilot*);
	void __openContextMenu(const AirportObject*);
	
	/**
	 * Loads necessary icons.
	 */
	void __loadIcon(const QString&, GLuint&);
	
	/**
	 * Saves/reads settings (zoom, camera Position, colors, etc etc).
	 */
	void __storeSettings();
	void __restoreSettings();
	
	inline double __distanceFromCamera(const double& _x, const double& _y) {
		return sqrt(
			pow(_x - __lastMousePosInterpolated.x(), 2) +
			pow(_y - __lastMousePosInterpolated.y(), 2)
		);
	}
	
	/* OpenGL's textures. */
	GLuint	__apIcon;
	GLuint	__pilotIcon;
	GLuint	__towerIcon;
	
	/* These variables are needed to render our textures. */
	GLdouble	__vertices[8];
	GLdouble	__texCoords[8];
	
	/* Camera position x, y */
	QPointF		__position;
	
	/* Zoom factor */
	int			__zoom;
	
	/* Last mouse position */
	QPoint	__lastMousePos;
	
	/* And last mouse position interpolated to ortho range */
	QPointF __lastMousePosInterpolated;
	
	/* Font being used to display labels */
	QFont	__pilotsFont;
	QFont	__apsFont;
	
	/* Stores window geometry */
	int		__winWidth;
	int		__winHeight;
	
	/* Stores glOrtho ranges */
	GLdouble	__orthoRangeX;
	GLdouble	__orthoRangeY;
	
	const Clickable * __underMouse;
	const Pilot *		__tracked;
	
	/* Where to display latitude and longitude of mouse position */
	QLabel *	__label;
	
	bool		__toolTipWasShown;
	
	VatsinatorApplication &	__mother;
	
	VatsimDataHandler & __data;
	
	const AirportsMap & __airports;
	
	MetarsWindow * __metars;
	FlightDetailsWindow * __flightDetails;
	
	
	
};

#endif // OPENGLWIDGET_H
