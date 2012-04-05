/*
    OpenGLWidget.cpp
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

#include "../include/OpenGLWidget.h"

#include "../include/VatsinatorApplication.h"
#include "../include/MetarAction.h"
#include "../include/MetarsWindow.h"
#include "../include/AirportsDatabase.h"
#include "../include/TrackAction.h"
#include "../include/UserInterface.h"
#include "../include/FlightDetailsWindow.h"
#include "../include/FlightDetailsAction.h"

#include "../include/defines.h"

template< typename T >
inline T absHelper(const T& _v) {
	if (_v < 0)
		return -_v;
	return _v;
}

OpenGLWidget::OpenGLWidget(QWidget* _parent) :
		QGLWidget(_parent),
		__vertices({-0.025, -0.025, -0.025, 0.025, 0.025, 0.025, 0.025, -0.025}),
		__texCoords({0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0}),
		__position(0.0, 0.0),
		__zoom(ZOOM_MINIMUM),
		__underMouse(NULL),
		__tracked(NULL),
		__label(NULL),
		__toolTipWasShown(false),
		__mother(VatsinatorApplication::GetSingleton()),
		__data(VatsimDataHandler::GetSingleton()),
		__airports(__data.getActiveAirports()) {
	__apsFont.setPointSize(__apsFont.pointSize() - 1);
}

OpenGLWidget::~OpenGLWidget() {
	__storeSettings();
}

void
OpenGLWidget::init() {
	setEnabled(true);
	
	glEnable(GL_TEXTURE_2D);
	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	
	__loadIcon(AIRPORT_ICON, __apIcon);
	__loadIcon(PILOT_ICON, __pilotIcon);
	__loadIcon(AIRPORT_STAFFED_ICON, __apStaffedIcon);
	
	__metars = MetarsWindow::GetSingletonPtr();
	__flightDetails = FlightDetailsWindow::GetSingletonPtr();
	
	__restoreSettings();
}

void
OpenGLWidget::initializeGL() {
	glShadeModel(GL_SMOOTH);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glEnable(GL_LINE_STIPPLE);
	
	init();
}

void
OpenGLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	if (__tracked) {
		__position.rx() = __tracked->position.longitude / 180;
		__position.ry() = __tracked->position.latitude / 90;
	}
	
	__prepareMatrix(AIRPORTS_PILOTS);
	
	__drawAirports();
	__drawPilots();
	
	__drawLines();
	
	if (!__toolTipWasShown) {
		QToolTip::hideText();
		__underMouse = NULL;
		if (cursor().shape() != Qt::SizeAllCursor)
			setCursor(QCursor(Qt::ArrowCursor));
	}
	
	__toolTipWasShown = false;
	
	swapBuffers();
}

void
OpenGLWidget::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);
	
	__orthoRangeX = (GLdouble)width / BASE_SIZE_WIDTH;
	__orthoRangeY = (GLdouble)height / BASE_SIZE_HEIGHT;
	
	__prepareMatrix(WORLD);
	
	__winWidth = width;
	__winHeight = height;
}

void
OpenGLWidget::wheelEvent(QWheelEvent* _event) {
	int steps = _event->delta() / 120;
	
	if (__zoom + steps <= ZOOM_MINIMUM)
		__zoom = ZOOM_MINIMUM;
	else
		__zoom += steps;
	
	_event->accept();
	
	paintGL();
}

void
OpenGLWidget::mousePressEvent(QMouseEvent* _event) {
	__lastMousePos = _event->pos();
	
	if (_event->buttons() & Qt::RightButton) {
		if (__underMouse && __underMouse->objectType() == PLANE)
			__openContextMenu(static_cast< const Pilot* >(__underMouse));
		else if (__underMouse && __underMouse->objectType() == AIRPORT)
			__openContextMenu(static_cast< const AirportObject* >(__underMouse));
	} else if (_event->buttons() & Qt::LeftButton) {
		if (__underMouse && __underMouse->objectType() == PLANE)
			__flightDetails->showWindow(static_cast< const Pilot* >(__underMouse));
		else if (__underMouse && __underMouse->objectType() == AIRPORT)
			__metars->showWindow(static_cast< const AirportObject* >(__underMouse)->getData()->icao);
	}
	
	//paintGL();
}

void
OpenGLWidget::mouseReleaseEvent(QMouseEvent*) {
	setCursor(QCursor(Qt::ArrowCursor));
}

void
OpenGLWidget::mouseMoveEvent(QMouseEvent* _event) {
	int dx = _event->x() - __lastMousePos.x();
	int dy = _event->y() - __lastMousePos.y();
	
	if (_event->buttons() & Qt::LeftButton) {
		setCursor(QCursor(Qt::SizeAllCursor));
		__tracked = NULL;
		
		// count the new position
		__position.rx() -= (double)dx / 400.0 / (double)__zoom;
		__position.ry() += (double)dy / 300.0 / (double)__zoom;
	}
	
	__lastMousePos = _event->pos();
	
	// counts the mouse position point, interpolated to the GL's context size
	__lastMousePosInterpolated.rx() = ((double)__lastMousePos.x() -
			(((double)__winWidth - BASE_SIZE_WIDTH) / 2)) / (BASE_SIZE_WIDTH / 2) - 1.0;
	__lastMousePosInterpolated.ry() = -(((double)__lastMousePos.y() -
			(((double)__winHeight -BASE_SIZE_HEIGHT) / 2)) / (BASE_SIZE_HEIGHT / 2) - 1.0);
	
	// count the mouse position global coordinates
	double longitude = (__lastMousePosInterpolated.x() / (double)__zoom + __position.x()) * 180;
	double latitude = (__lastMousePosInterpolated.y() / (double)__zoom + __position.y()) * 90;
	
	// update the label on the very bottom of the main window
	UserInterface::GetSingleton().getPositionBox()->setText(
			QString((latitude > 0) ? "N" : "S") + " " +
			QString::number(absHelper(latitude), 'g', 6) + " " + 
			QString((longitude < 0) ? "W" : "E") + " " +
			QString::number(absHelper(longitude), 'g', 6) + " "
		);
	
	paintGL();
}

void
OpenGLWidget::keyPressEvent(QKeyEvent* _event) {
	switch (_event->key()) {
		case Qt::Key_PageUp:
			__zoom += 1;
			break;
		case Qt::Key_PageDown:
			if (__zoom > ZOOM_MINIMUM)
				__zoom -= 1;
			break;
	}
	
	paintGL();
}

void
OpenGLWidget::__prepareMatrix(PMMatrixMode _mode) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	switch(_mode) {
		case AIRPORTS_PILOTS:
			glOrtho(
					-__orthoRangeX,
		 			__orthoRangeX,
		 			-__orthoRangeY,
		 			__orthoRangeY,
		 			1.0, -1.0
				);
			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			
			break;
		case WORLD:
			glOrtho(
					-180, 180,
					-90, 90,
					1.0, -1.0
				);
			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			
			glScalef(__zoom, __zoom, 0.0);
			glTranslated(-__position.x(), __position.y(), 0.0);
			
			break;
	}
}

void
OpenGLWidget::__drawAirports() {
	
	glColor4f(COLOR_BACKGROUND);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_DOUBLE, 0, __vertices);
	
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_DOUBLE, 0, __texCoords);
	
	for (auto it = __airports.begin(); it != __airports.end(); ++it) {
		
		if (!it.value()->getData())
			continue;
		
		GLdouble x = it.value()->getData()->longitude / 180;
		x -= __position.x();
		x *= __zoom;
		
		if (x < -__orthoRangeX || x > __orthoRangeX)
			continue;
		
		GLdouble y = it.value()->getData()->latitude / 90;
		y -= __position.y();
		y *= __zoom;
		
		if (y < -__orthoRangeY || y > __orthoRangeY)
			continue;
		
		bool inRange = __distanceFromCamera(x, y) < OBJECT_TO_MOUSE;
		
		if (inRange)
			setCursor(QCursor(Qt::PointingHandCursor));
		
		glBindTexture(GL_TEXTURE_2D, (it.value()->getStuff().isEmpty()) ? __apIcon : __apStaffedIcon );
		
		glPushMatrix();
		
			glColor4f(1.0, 1.0, 1.0, 1.0);
			
			glTranslated(x, y, 0);
			glDrawArrays(GL_QUADS, 0, 4);
			
			glColor4f(AP_LABEL_COLOR);
			renderText(-0.03, 0.04, 0.3, it.key(), __apsFont, 64);
			
			if (inRange) {
				__toolTipWasShown = true;
				__underMouse = it.value();
				
				QString tooltipText = QString("<center>") + it.key() + "<br><nobr>";
				tooltipText.append(it.value()->getData()->name);
				tooltipText.append(", ");
				tooltipText.append(it.value()->getData()->city);
				tooltipText.append("</nobr>");
				
				for (const Controller* c: it.value()->getStuff()) {
					tooltipText.append("<br><nobr>");
					tooltipText.append(c->callsign + " " + c->frequency + " " + c->realName);
					tooltipText.append("</nobr>");
				}
				
				tooltipText.append("<br>Departures: ");
				tooltipText.append(QString::number(it.value()->countDepartures()));
				
				tooltipText.append("<br>Arrivals: ");
				tooltipText.append(QString::number(it.value()->countArrivals()));
				
				tooltipText.append("</center>");
				
				if (!QToolTip::isVisible()) {
					QToolTip::showText(mapToGlobal(__lastMousePos),
							tooltipText,
							this
						);
				}
			}
			
		glPopMatrix();
	}
	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void
OpenGLWidget::__drawPilots() {
	const QVector< Pilot* > & pilots = VatsimDataHandler::GetSingleton().getPilots();
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, __pilotIcon);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_DOUBLE, 0, __vertices);
	
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_DOUBLE, 0, __texCoords);
	
	for (const Pilot* client: pilots) {
		if (client->flightStatus != AIRBORNE)
			continue;
		
		GLdouble x = client->position.longitude / 180;
		x -= __position.x();
		x *= __zoom;
		
		if (x < -__orthoRangeX || x > __orthoRangeX)
			continue;
		
		GLdouble y = client->position.latitude / 90;
		y -= __position.y();
		y *= __zoom;
		
		if (y < -__orthoRangeY || y > __orthoRangeY)
			continue;
		
		glPushMatrix();
			bool inRange = __distanceFromCamera(x, y) < OBJECT_TO_MOUSE;
			
			glColor4f(1.0, 1.0, 1.0, 1.0);
			
			glTranslated(x, y, 0);
			
			glPushMatrix();
				glRotatef((GLfloat)client->heading, 0, 0, -1);
				
				if (inRange)
					glScalef(1.3, 1.3, 1.0);
				
				glDrawArrays(GL_QUADS, 0, 4);
			glPopMatrix();
			
			glColor4f(PILOT_LABEL_COLOR);
			renderText(0.03, -0.01, 0.5, client->callsign, __pilotsFont, 64);
			
			if (inRange && !__toolTipWasShown) {
				__toolTipWasShown = true;
				__underMouse = client;
				setCursor(QCursor(Qt::PointingHandCursor));
				
				QString origin = (__airports[client->route.origin]->getData()) ?
						client->route.origin + " " + __airports[client->route.origin]->getData()->city :
						client->route.origin;
				
				QString destination = (__airports[client->route.destination]->getData()) ?
						client->route.destination + " " + __airports[client->route.destination]->getData()->city :
						client->route.destination;
				
				if (!QToolTip::isVisible())
					QToolTip::showText(mapToGlobal(__lastMousePos),
									   "<center>" + client->callsign + "<br><nobr>" +
									   client->realName + " (" + client->aircraft +
									   ")</nobr><br><nobr>" + origin +
									   " > " + destination + "</nobr><br>" +
									   "Ground speed: " + QString::number(client->groundSpeed) +
									   " kts<br>Altitude: " + QString::number(client->altitude) +
									   " ft</center>",
						this
					);
			}
		glPopMatrix();
	}
	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void
OpenGLWidget::__drawLines() {
	if (!__underMouse)
		return;
	
	glColor4f(LINES_COLOR);
	if (__underMouse->objectType() == PLANE) {
		const Pilot* pilot = static_cast< const Pilot* >(__underMouse);
		const Airport* ap = __airports[pilot->route.origin]->getData();
		
		if (ap) {
			glBegin(GL_LINES);
				glVertex3d(
						(ap->longitude / 180 - __position.x()) * __zoom,
						(ap->latitude / 90 - __position.y()) * __zoom,
						-0.9
					);
				glVertex3d(
						(pilot->position.longitude / 180 - __position.x()) * __zoom,
						(pilot->position.latitude / 90 - __position.y()) * __zoom,
						-0.9
					);
			glEnd();
		}
		
		ap = __airports[pilot->route.destination]->getData();
		if (ap) {
			glLineStipple(1, 0xF0F0);
			glBegin(GL_LINES);
				glVertex3d(
						(ap->longitude / 180 - __position.x()) * __zoom,
						(ap->latitude / 90 - __position.y()) * __zoom,
						-0.9
					);
				glVertex3d(
						(pilot->position.longitude / 180 - __position.x()) * __zoom,
						(pilot->position.latitude / 90 - __position.y()) * __zoom,
						-0.9
					);
			glEnd();
			glLineStipple(1, 0xFFFF);
			
		}
	} else if (__underMouse->objectType() == AIRPORT) {
		const AirportObject* ap = static_cast< const AirportObject* >(__underMouse);
		if (!ap->getData())
			return;
		
		for (const Pilot* p: ap->getOutbounds()) {
			glBegin(GL_LINES);
				glVertex3d(
						(p->position.longitude / 180 - __position.x()) * __zoom,
						(p->position.latitude / 90 - __position.y()) * __zoom,
						-0.9
					);
				glVertex3d(
						(ap->getData()->longitude / 180 - __position.x()) * __zoom,
						(ap->getData()->latitude / 90 - __position.y()) * __zoom,
						-0.9
					);
			glEnd();
		}
		
		glLineStipple(1, 0xF0F0);
		for (const Pilot* p: ap->getInbounds()) {
			glBegin(GL_LINES);
				glVertex3d(
						(p->position.longitude / 180 - __position.x()) * __zoom,
						(p->position.latitude / 90 - __position.y()) * __zoom,
						-0.9
					);
				glVertex3d(
						(ap->getData()->longitude / 180 - __position.x()) * __zoom,
						(ap->getData()->latitude / 90 - __position.y()) * __zoom,
						-0.9
					);
			glEnd();
		}
		glLineStipple(1, 0xFFFF);
	}
}

void
OpenGLWidget::__openContextMenu(const Pilot* _pilot) {
	QMenu* dMenu = new QMenu(_pilot->callsign, this);
	
	FlightDetailsAction* showDetails = new FlightDetailsAction(_pilot, this);
	TrackAction* trackThisFlight = new TrackAction(_pilot, this);
	MetarAction* showDepMetar = new MetarAction(_pilot->route.origin, this);
	MetarAction* showArrMetar = new MetarAction(_pilot->route.destination, this);
	
	dMenu->addAction(showDetails);
	dMenu->addAction(trackThisFlight);
	dMenu->addSeparator();
	dMenu->addAction(showDepMetar);
	dMenu->addAction(showArrMetar);
	
	connect(showDetails, SIGNAL(clicked(const Client*)), __flightDetails, SLOT(showWindow(const Client*)));
	connect(trackThisFlight, SIGNAL(clicked(const Pilot*)), this, SLOT(trackFlight(const Pilot*)));
	connect(showDepMetar, SIGNAL(clicked(QString)), __metars, SLOT(showWindow(QString)));
	connect(showArrMetar, SIGNAL(clicked(QString)), __metars, SLOT(showWindow(QString)));
	
	dMenu->exec(mapToGlobal(__lastMousePos));
	delete dMenu;
}

void
OpenGLWidget::__openContextMenu(const AirportObject* _ap) {
	QMenu* dMenu = new QMenu(_ap->getData()->icao, this);
	
	MetarAction* showMetar = new MetarAction(_ap->getData()->icao, this);
	
	dMenu->addAction(showMetar);
	connect(showMetar, SIGNAL(clicked(QString)), __metars, SLOT(showWindow(QString)));
	
	if (!_ap->getOutbounds().isEmpty() && _ap->countDepartures()) {
		dMenu->addSeparator();
		QAction* desc = new QAction("Departures", this);
		desc->setEnabled(false);
		dMenu->addAction(desc);
	}
	
	for (const Pilot* p: _ap->getOutbounds()) {
		if (p->flightStatus != DEPARTING)
			continue;
		FlightDetailsAction* showDetails = new FlightDetailsAction(p->callsign, p, this);
		dMenu->addAction(showDetails);
		connect(showDetails, SIGNAL(clicked(const Client*)), __flightDetails, SLOT(showWindow(const Client*)));
	}
	
	if (!_ap->getInbounds().isEmpty() && _ap->countArrivals()) {
		dMenu->addSeparator();
		QAction* desc = new QAction("Arrivals", this);
		desc->setEnabled(false);
		dMenu->addAction(desc);
	}
	
	for (const Pilot* p: _ap->getInbounds()) {
		if (p->flightStatus != ARRIVED)
			continue;
		FlightDetailsAction* showDetails = new FlightDetailsAction(p->callsign, p, this);
		dMenu->addAction(showDetails);
		connect(showDetails, SIGNAL(clicked(const Client*)), __flightDetails, SLOT(showWindow(const Client*)));
	}
	
	dMenu->exec(mapToGlobal(__lastMousePos));
	delete dMenu;
}

void
OpenGLWidget::__loadIcon(const QString& _fName, GLuint& _pix) {
	QImage final, temp;
	
	if (!temp.load(_fName)) {
		__mother.alert("Image could not be loaded!");
		__mother.quit();
	}
	
	final = QGLWidget::convertToGLFormat(temp);
	
	glEnable(GL_TEXTURE_2D);
	
	glGenTextures(1, &_pix);
	
	glBindTexture(GL_TEXTURE_2D, _pix);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, final.width(), final.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, final.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void
OpenGLWidget::__storeSettings() {
	QSettings settings("Vatsinator", "Vatsinator");
	
	settings.beginGroup("CameraSettings");
	
	settings.setValue("zoomFactor", __zoom);
	settings.setValue("cameraPosition", __position);
	
	settings.endGroup();
}

void
OpenGLWidget::__restoreSettings() {
	QSettings settings("Vatsinator", "Vatsinator");
	
	settings.beginGroup("CameraSettings");
	
	__zoom = settings.value("zoomFactor", ZOOM_MINIMUM).toInt();
	__position = settings.value("cameraPosition", QPointF(0.0, 0.0)).toPointF();

	settings.endGroup();
}
