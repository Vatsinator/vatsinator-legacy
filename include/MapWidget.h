/*
    MapWidget.h
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


#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QGLWidget>
#include <QLabel>

#include "VatsimDataHandler.h"
#include "Singleton.h"

class AirportDetailsWindow;
class ATCDetailsWindow;
class Clickable;
class Fir;
class FirsDatabase;
class SettingsWindow;
class VatsinatorApplication;
class MetarsWindow;
class FlightDetailsWindow;

enum PMMatrixMode {
	AIRPORTS_PILOTS, WORLD
};

class MapWidget :
		public QGLWidget,
		public Singleton< MapWidget >{
	
	/*
	 * We need this class to build our QGLWidget for UserInterface class.
	 */
	
	Q_OBJECT
	
public:
	MapWidget(QWidget* = 0);
	
	virtual ~MapWidget();
	
	void init();
	
	inline const Pilot *& getTrackedPilot() { return __tracked; }
	
	/* For Pilot class */
	inline const QImage & getPilotToolTipBackground() { return __pilotToolTip; }
	inline const QFont & getPilotFont() { return __pilotFont; }
	
	/* For AirportObject class */
	inline const QImage & getAirportToolTipBackground() { return __airportToolTip; }
	inline const QFont & getAirportFont() { return __airportFont; }
	
	/* For Fir class */
	inline const QImage & getFirToolTipBackground() { return __firToolTip; }
	inline const QFont & getFirFont() { return __firFont; }
	
	static GLuint loadImage(const QImage&);
	static GLuint loadImage(const QString&);
	static void deleteImage(GLuint);
	
signals:
	void contextMenuRequested(const Pilot*);
	void contextMenuRequested(const AirportObject*);
	void contextMenuRequested(const Fir*);
	void flightDetailsWindowRequested(const Client*);
	void airportDetailsWindowRequested(const AirportObject*);
	
public slots:
	void trackFlight(const Pilot* _p) { __tracked = _p; }
	void showPilot(const Pilot*);
	void redraw();
	
protected:
	/* Here we reimplement some functions that are needed for
	 * OpenGL to work properly. */
	void initializeGL();
	void paintGL();
	void resizeGL(int, int);
	void wheelEvent(QWheelEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void keyPressEvent(QKeyEvent*);
	void keyReleaseEvent(QKeyEvent*);
	
private slots:
	void __openContextMenu(const Pilot*);
	void __openContextMenu(const AirportObject*);
	void __openContextMenu(const Fir*);
	
private:
	void __prepareMatrix(PMMatrixMode);
	
	void __drawFirs();
	void __drawUirs();
	void __drawFirsLabels();
	void __drawAirports();
	void __drawPilots();
	void __drawLines(); // lines when airport/pilot on hover
	
	void __storeSettings();
	void __restoreSettings();
	
	void __produceCircle();
	
	inline
	double __distanceFromCamera(const double& _x, const double& _y) {
		return sqrt(
			pow(_x - __lastMousePosInterpolated.x(), 2) +
			pow(_y - __lastMousePosInterpolated.y(), 2)
		);
	}
	
	inline
	void __mapCoordinates(const double& _xFrom, const double& _yFrom,
						  double& _xTo, double& _yTo) {
		_xTo = (_xFrom / 180 - __position.x()) * __zoom;
		_yTo = (_yFrom / 90 - __position.y()) * __zoom;
	}
	
	/* OpenGL's textures. */
	GLuint	__apIcon;
	GLuint	__apStaffedIcon;
	GLuint	__pilotIcon;
	
	/* Used by Pilot class */
	QImage	__pilotToolTip;
	QFont	__pilotFont;
	
	/* For AirportObject class */
	QImage	__airportToolTip;
	QFont	__airportFont;
	
	/* For Fir class */
	QImage	__firToolTip;
	QFont	__firFont;
	
	/* Approach circle array and vertices count */
	GLdouble *	__circle;
	unsigned	__circleCount;
	
	/* Camera position x, y */
	QPointF		__position;
	
	/* Zoom factor */
	int		__zoom;
	
	/* Last mouse position */
	QPoint	__lastMousePos;
	
	/* And last mouse position interpolated to ortho range */
	QPointF __lastMousePosInterpolated;
	
	/* Showing pilots' labels or not? */
	bool	__keyPressed;
	
	/* Stores window geometry */
	int	__winWidth;
	int	__winHeight;
	
	/* Stores glOrtho ranges */
	GLdouble	__orthoRangeX;
	GLdouble	__orthoRangeY;
	
	/* Clickable object under mouse and tracked pilot pointers */
	const Clickable *	__underMouse;
	const Pilot *		__tracked;
	
	/* Where to display latitude and longitude of mouse position */
	QLabel *	__label;
	
	/* To prevent from showing tooltip when it is not recommended */
	bool		__toolTipWasShown; // during current frame render
	bool		__contextMenuActive;
	bool		__refreshLaterFlag;
	
	VatsinatorApplication &	__mother;
	
	VatsimDataHandler & __data;
	
	const AirportsMap & __airports;
	
	FirsDatabase *		__firs;
	AirportDetailsWindow *	__airportDetailsWindow;
	ATCDetailsWindow *	__atcDetailsWindow;
	FlightDetailsWindow *	__flightDetailsWindow;
	MetarsWindow *		__metarsWindow;
	SettingsWindow *	__settings;
};

#endif // MAPWIDGET_H
