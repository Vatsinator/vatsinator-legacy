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
#include <cstdlib>

#include "../include/MapWidget.h"

#include "../include/AirportsDatabase.h"
#include "../include/AirportDetailsWindow.h"
#include "../include/ATCDetailsWindow.h"
#include "../include/ClientDetailsAction.h"
#include "../include/FirsDatabase.h"
#include "../include/FlightDetailsWindow.h"
#include "../include/MetarAction.h"
#include "../include/MetarsWindow.h"
#include "../include/SettingsWindow.h"
#include "../include/TrackAction.h"
#include "../include/UserInterface.h"
#include "../include/VatsinatorApplication.h"

#include "../include/defines.h"

const double PI = 3.1415926535897;

const GLdouble VERTICES[] = {-0.025, -0.025, -0.025, 0.025, 0.025, 0.025, 0.025, -0.025};
const GLdouble TEXCOORDS[] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0};

template< typename T >
inline T absHelper(const T& _v) {
	if (_v < 0)
		return -_v;
	return _v;
}

MapWidget::MapWidget(QWidget* _parent) :
		QGLWidget(_parent),
		__position(0.0, 0.0),
		__zoom(ZOOM_MINIMUM),
		__keyPressed(false),
		__underMouse(NULL),
		__tracked(NULL),
		__label(NULL),
		__toolTipWasShown(false),
		__mother(VatsinatorApplication::GetSingleton()),
		__data(VatsimDataHandler::GetSingleton()),
		__airports(__data.getActiveAirports()) {
	__apsFont.setPointSize(__apsFont.pointSize() - 1);
	__produceCircle();
	
	connect(VatsinatorApplication::GetSingletonPtr(),	SIGNAL(glRepaintNeeded()),
		this,						SLOT(updateGL()));
	
	setAutoBufferSwap(true);
}

MapWidget::~MapWidget() {
	__storeSettings();
	delete [] __circle;
}

void
MapWidget::init() {
	setEnabled(true);
	
	glEnable(GL_TEXTURE_2D);
	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	
	__loadIcon(":/pixmaps/airport.png", __apIcon);
	__loadIcon(":/pixmaps/airport_staffed.png", __apStaffedIcon);
	__loadIcon(":/pixmaps/plane.png", __pilotIcon);
	
	__firs = FirsDatabase::GetSingletonPtr();
	__airportDetails = AirportDetailsWindow::GetSingletonPtr();
	__atcDetails = ATCDetailsWindow::GetSingletonPtr();
	__metars = MetarsWindow::GetSingletonPtr();
	__flightDetails = FlightDetailsWindow::GetSingletonPtr();
	__settings = SettingsWindow::GetSingletonPtr();
	
	__restoreSettings();
}

void
MapWidget::initializeGL() {
	glShadeModel(GL_SMOOTH);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glEnable(GL_LINE_STIPPLE);
	
	init();
}

void
MapWidget::paintGL() {
	glClearColor(
		__settings->getBackgroundColor().redF(),
		__settings->getBackgroundColor().greenF(),
		__settings->getBackgroundColor().blueF(),
		__settings->getBackgroundColor().alphaF());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
	
	if (__tracked) {
		__position.rx() = __tracked->position.longitude / 180;
		__position.ry() = __tracked->position.latitude / 90;
	}
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	__prepareMatrix(WORLD);
	if (__settings->firsLayerOn())
		__drawFirs();
	
	if (__settings->uirsLayerOn())
		__drawUirs();
	
	__prepareMatrix(AIRPORTS_PILOTS);
	
	glVertexPointer(2, GL_DOUBLE, 0, VERTICES);
	
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_DOUBLE, 0, TEXCOORDS);
	
	if (__settings->airportsLayerOn())
		__drawAirports();
	
	if (__settings->pilotsLayerOn())
		__drawPilots();
	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	__drawLines();
	
	if (!__toolTipWasShown) {
		QToolTip::hideText();
		__underMouse = NULL;
		if (cursor().shape() != Qt::SizeAllCursor)
			setCursor(QCursor(Qt::ArrowCursor));
	}
	
	__toolTipWasShown = false;
}

void
MapWidget::showPilot(const Pilot* _p) {
	__position.rx() = _p->position.longitude / 180;
	__position.ry() = _p->position.latitude / 90;
	updateGL();
}

void
MapWidget::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);
	
	__orthoRangeX = (GLdouble)width / BASE_SIZE_WIDTH;
	__orthoRangeY = (GLdouble)height / BASE_SIZE_HEIGHT;
	
	__prepareMatrix(WORLD);
	
	__winWidth = width;
	__winHeight = height;
}

void
MapWidget::wheelEvent(QWheelEvent* _event) {
	int steps = _event->delta() / 120;
	
	if (__zoom + steps <= ZOOM_MINIMUM)
		__zoom = ZOOM_MINIMUM;
	else
		__zoom += steps;
	
	_event->accept();
	
	updateGL();
}

void
MapWidget::mousePressEvent(QMouseEvent* _event) {
	__lastMousePos = _event->pos();
	
	if ((_event->buttons() & Qt::RightButton) && __underMouse) {
		switch (__underMouse->objectType()) {
			case PLANE:
				__openContextMenu(static_cast< const Pilot* >(__underMouse));
				break;
			case AIRPORT:
				__openContextMenu(static_cast< const AirportObject* >(__underMouse));
				break;
			case FIR:
				__openContextMenu(static_cast< const Fir* >(__underMouse));
				break;
			case UIR:
				break;
		}
	} else if ((_event->buttons() & Qt::LeftButton) && __underMouse) {
		switch (__underMouse->objectType()) {
			case PLANE:
				__flightDetails->showWindow(static_cast< const Pilot* >(__underMouse));
				break;
			case AIRPORT:
				__airportDetails->showWindow(static_cast< const AirportObject* >(__underMouse));
				break;
			case FIR:
				//__atcDetails->showWindow(static_cast< const Fir* >(__underMouse));
				break;
			case UIR:
				break;
		}
	}
}

void
MapWidget::mouseReleaseEvent(QMouseEvent*) {
	setCursor(QCursor(Qt::ArrowCursor));
}

void
MapWidget::mouseMoveEvent(QMouseEvent* _event) {
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
	
	updateGL();
}

void
MapWidget::keyPressEvent(QKeyEvent* _event) {
	switch (_event->key()) {
		case Qt::Key_PageUp:
			__zoom += 1;
			break;
		case Qt::Key_PageDown:
			if (__zoom > ZOOM_MINIMUM)
				__zoom -= 1;
			break;
		case Qt::Key_Control:
			__keyPressed = true;
	}
	
	updateGL();
}

void
MapWidget::keyReleaseEvent(QKeyEvent* _event) {
	switch (_event->key()) {
		case Qt::Key_Control:
			__keyPressed = false;
			break;
	}
	
	updateGL();
}

void
MapWidget::__prepareMatrix(PMMatrixMode _mode) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(
		-__orthoRangeX,
		 __orthoRangeX,
		 -__orthoRangeY,
		 __orthoRangeY,
		 2.0, -2.0
	);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	switch(_mode) {
		case WORLD:
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			
			glScaled((double)1/(double)180, (double)1/(double)90, 0.0);
			glScaled(__zoom, __zoom, 0.0);
			glTranslated(-__position.x() * 180, -__position.y() * 90, 0.0);
			
			break;
		case AIRPORTS_PILOTS:
			break;
	}
}

void
MapWidget::__drawFirs() {
	for (const Fir& fir: __firs->getFirs()) {
		glPushMatrix();
		if (!fir.getStaff().isEmpty()) {
			glColor4f(
				__settings->getStaffedFirBordersColor().redF(),
				__settings->getStaffedFirBordersColor().greenF(),
				__settings->getStaffedFirBordersColor().blueF(),
				__settings->getStaffedFirBordersColor().alphaF());
			glLineWidth(3.0);
			glTranslated(0.0, 0.0, 0.1);
		} else {
			glColor4f(
				__settings->getUnstaffedFirBordersColor().redF(),
				__settings->getUnstaffedFirBordersColor().greenF(),
				__settings->getUnstaffedFirBordersColor().blueF(),
				__settings->getUnstaffedFirBordersColor().alphaF());
		}
		
		glVertexPointer(2, GL_DOUBLE, 0, &fir.coords[0].x);
		glDrawArrays(GL_LINE_LOOP, 0, fir.coords.size());
		
		double x, y;
		__mapCoordinates(fir.header.textPosition.x, fir.header.textPosition.y, x, y);
		if ((x <= __orthoRangeX) && (y <= __orthoRangeY) &&
				(x >= -__orthoRangeX) && (y >= -__orthoRangeY)) {
			QString icao(fir.header.icao);
			if (icao.length() > 4) {
				icao = icao.left(4);
				icao += " Oceanic";
			}
		
			renderText(fir.header.textPosition.x - 0.5, fir.header.textPosition.y, 0.0,
					   icao, __apsFont, 64);
			if (__distanceFromCamera(x, y) < OBJECT_TO_MOUSE && !__toolTipWasShown) {
				__toolTipWasShown = true;
				setCursor(QCursor(Qt::PointingHandCursor));
				__underMouse = &fir;
				
				QString tooltipText = QString("<center>") + icao;
				if (!fir.name.isEmpty()) {
					tooltipText.append("<br><nobr>");
					tooltipText.append(fir.name);
					tooltipText.append("</nobr>");
				}
				
				for (const Controller* c: fir.getStaff()) {
					tooltipText.append("<br><nobr>");
					tooltipText.append(c->callsign + " " + c->frequency + " " + c->realName);
					tooltipText.append("</nobr>");
				}
				
				QToolTip::showText(mapToGlobal(__lastMousePos),
						tooltipText,
						this
					);
			}
		}
		glLineWidth(1.0);
		glPopMatrix();
	}
}

void
MapWidget::__drawUirs() {
	for (const Uir* uir: __data.getUIRs()) {
		if (!uir->getStaff().isEmpty()) {
			glPushMatrix();
				glTranslatef(0.0, 0.0, 0.1);
				glColor4f(
					__settings->getStaffedUirBordersColor().redF(),
					__settings->getStaffedUirBordersColor().greenF(),
					__settings->getStaffedUirBordersColor().blueF(),
					__settings->getStaffedUirBordersColor().alphaF());
				glLineWidth(3.0);
				for (const Fir* fir: uir->getRange()) {
					glVertexPointer(2, GL_DOUBLE, 0, &fir->coords[0].x);
					glDrawArrays(GL_LINE_LOOP, 0, fir->coords.size());
				}
				glLineWidth(1.0);
			glPopMatrix();
		}
	}
}

void
MapWidget::__drawAirports() {
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
		
		glBindTexture(GL_TEXTURE_2D, (it.value()->getStaff().isEmpty()) ? __apIcon : __apStaffedIcon );
		
		glPushMatrix();
		
			glColor4f(1.0, 1.0, 1.0, 1.0);
			
			glTranslated(x, y, 0);
			glDrawArrays(GL_QUADS, 0, 4);
	
			glColor4f(
				__settings->getAirportsLabelsColor().redF(),
				__settings->getAirportsLabelsColor().greenF(),
				__settings->getAirportsLabelsColor().blueF(),
				__settings->getAirportsLabelsColor().alphaF());
			renderText(-0.03, 0.04, -1.0, it.key(), __apsFont, 64);
			
			unsigned deps = it.value()->countDepartures();
			unsigned arrs = it.value()->countArrivals();
			
			if (inRange) {
				__toolTipWasShown = true;
				__underMouse = it.value();
				
				QString tooltipText = QString("<center>") + it.key() + "<br><nobr>";
				tooltipText.append(it.value()->getData()->name);
				tooltipText.append(", ");
				tooltipText.append(it.value()->getData()->city);
				tooltipText.append("</nobr>");
				
				for (const Controller* c: it.value()->getStaff()) {
					tooltipText.append("<br><nobr>");
					tooltipText.append(c->callsign + " " + c->frequency + " " + c->realName);
					tooltipText.append("</nobr>");
				}
				
				if (deps) {
					tooltipText.append("<br>Departures: ");
					tooltipText.append(QString::number(deps));
				}
				
				if (arrs) {
					tooltipText.append("<br>Arrivals: ");
					tooltipText.append(QString::number(arrs));
				}
				
				tooltipText.append("</center>");
				
				QToolTip::showText(mapToGlobal(__lastMousePos),
						tooltipText,
						this
					);
			}
			
			if (it.value()->hasApproach()) {
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glBindTexture(GL_TEXTURE_2D, 0);
				
				glColor4f(
					__settings->getApproachCircleColor().redF(),
					__settings->getApproachCircleColor().greenF(),
					__settings->getApproachCircleColor().blueF(),
					__settings->getApproachCircleColor().alphaF());
				glVertexPointer(2, GL_DOUBLE, 0, __circle);
				
				glLineWidth(1.5);
				glLineStipple(1, 0xF0F0);
				glPushMatrix();
					glScaled(0.005 * __zoom, 0.005 * __zoom, 0);
					glDrawArrays(GL_LINE_LOOP, 0, __circleCount);
				glPopMatrix();
				glLineWidth(1.0);
				glLineStipple(1, 0xFFFF);
				
				glVertexPointer(2, GL_DOUBLE, 0, VERTICES);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			}
			
		glPopMatrix();
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void
MapWidget::__drawPilots() {
	const QVector< Pilot* > & pilots = VatsimDataHandler::GetSingleton().getPilots();
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, __pilotIcon);
	
	
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
			
			glTranslated(x, y, -0.1);
			
			glPushMatrix();
				glRotatef((GLfloat)client->heading, 0, 0, -1);
				
				if (inRange && !__toolTipWasShown)
					glScalef(1.3, 1.3, 1.0);
				
				glDrawArrays(GL_QUADS, 0, 4);
				
			glPopMatrix();
			
			glColor4f(
				__settings->getPilotsLabelsColor().redF(),
				__settings->getPilotsLabelsColor().greenF(),
				__settings->getPilotsLabelsColor().blueF(),
				__settings->getPilotsLabelsColor().alphaF());
			
			if (((__settings->getPilotsLabelsSettings() & WHEN_HOVERED)
					&& (__keyPressed || inRange))
					|| (__settings->getPilotsLabelsSettings() & ALWAYS))
				renderText(0.03, -0.01, -1.5, client->callsign, __pilotsFont, 64);
			
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
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void
MapWidget::__drawLines() {
	if (!__underMouse)
		return;
	
	glEnableClientState(GL_VERTEX_ARRAY);
	if (__underMouse->objectType() == PLANE) {
		glColor4f(LINES_COLOR);
		const Pilot* pilot = static_cast< const Pilot* >(__underMouse);
		const Airport* ap = __airports[pilot->route.origin]->getData();
		
		if (ap) {
			GLdouble vertices[] = {
					(ap->longitude / 180 - __position.x()) * __zoom,
					(ap->latitude / 90 - __position.y()) * __zoom,
					(pilot->position.longitude / 180 - __position.x()) * __zoom,
					(pilot->position.latitude / 90 - __position.y()) * __zoom
				};
			
			glVertexPointer(2, GL_DOUBLE, 0, vertices);
			glDrawArrays(GL_LINES, 0, 4);
		}
		
		ap = __airports[pilot->route.destination]->getData();
		if (ap) {
			GLdouble vertices[] = {
					(ap->longitude / 180 - __position.x()) * __zoom,
					(ap->latitude / 90 - __position.y()) * __zoom,
					(pilot->position.longitude / 180 - __position.x()) * __zoom,
					(pilot->position.latitude / 90 - __position.y()) * __zoom
				};
				
			glVertexPointer(2, GL_DOUBLE, 0, vertices);
			glLineStipple(1, 0xF0F0);
			glDrawArrays(GL_LINES, 0, 4);
			glLineStipple(1, 0xFFFF);
			
		}
	} else if (__underMouse->objectType() == AIRPORT) {
		const AirportObject* ap = static_cast< const AirportObject* >(__underMouse);
		if (!ap->getData())
			return;
		
		unsigned linesOut = ap->getOutbounds().size() - ap->countDepartures();
		unsigned linesIn = ap->getInbounds().size() - ap->countArrivals();
		
		GLdouble* vertices = new GLdouble[(linesOut + linesIn) * 4];
		unsigned i = 0;
		for (const Pilot* p: ap->getOutbounds()) {
			if (p->flightStatus == DEPARTING)
				continue;
			
			/* We cannot use vertices[i++], vertices[i++], as the result of such
			 * called function is undefined (according to C++ standard) */
			__mapCoordinates(p->position.longitude, p->position.latitude,
							 vertices[i], vertices[i+1]);
			if ((__settings->getPilotsLabelsSettings() & AIRPORT_RELATED)
					&& (p->flightStatus != ARRIVED && !__keyPressed))
				renderText(vertices[i] + 0.03, vertices[i+1] - 0.01, -1.5, p->callsign, __pilotsFont, 64);
			i += 2;
			
			__mapCoordinates(ap->getData()->longitude, ap->getData()->latitude,
							 vertices[i], vertices[i+1]);
			
			i += 2;
		}
		for (const Pilot* p: ap->getInbounds()) {
			if (p->flightStatus == ARRIVED)
				continue;
			
			__mapCoordinates(p->position.longitude, p->position.latitude,
							 vertices[i], vertices[i+1]);
			
			if ((__settings->getPilotsLabelsSettings() & AIRPORT_RELATED)
				&& (p->flightStatus != DEPARTING && !__keyPressed))
				renderText(vertices[i] + 0.03, vertices[i+1] - 0.01, -1.5, p->callsign, __pilotsFont, 64);
			i += 2;
			
			__mapCoordinates(ap->getData()->longitude, ap->getData()->latitude,
							 vertices[i], vertices[i+1]);
			
			i += 2;
		}
		
		glVertexPointer(2, GL_DOUBLE, 0, vertices);
		glColor4f(LINES_COLOR);
		
		glDrawArrays(GL_LINES, 0, linesOut * 2);
		
		glLineStipple(1, 0xF0F0);
		glDrawArrays(GL_LINES, linesOut * 2, linesIn * 2);
		glLineStipple(1, 0xFFFF);
		
		delete [] vertices;
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}

void
MapWidget::__openContextMenu(const Pilot* _pilot) {
	QMenu* dMenu = new QMenu(_pilot->callsign, this);
	
	ClientDetailsAction* showDetails = new ClientDetailsAction(_pilot, "Flight details", this);
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
MapWidget::__openContextMenu(const AirportObject* _ap) {
	QMenu* dMenu = new QMenu(_ap->getData()->icao, this);
	
	MetarAction* showMetar = new MetarAction(_ap->getData()->icao, this);
	
	dMenu->addAction(showMetar);
	connect(showMetar, SIGNAL(clicked(QString)), __metars, SLOT(showWindow(QString)));
	
	if (!_ap->getStaff().isEmpty()) {
		dMenu->addSeparator();
		QAction* desc = new QAction("Controllers", this);
		desc->setEnabled(false);
		dMenu->addAction(desc);
		
		for (const Controller* c: _ap->getStaff()) {
			ClientDetailsAction* showDetails = new ClientDetailsAction(c, c->callsign, this);
			dMenu->addAction(showDetails);
			connect(showDetails, SIGNAL(clicked(const Client*)), __atcDetails, SLOT(showWindow(const Client*)));
		}
	}
	
	if (!_ap->getOutbounds().isEmpty() && _ap->countDepartures()) {
		dMenu->addSeparator();
		QAction* desc = new QAction("Departures", this);
		desc->setEnabled(false);
		dMenu->addAction(desc);
		
		for (const Pilot* p: _ap->getOutbounds()) {
			if (p->flightStatus != DEPARTING)
				continue;
			ClientDetailsAction* showDetails = new ClientDetailsAction(p, p->callsign, this);
			dMenu->addAction(showDetails);
			connect(showDetails, SIGNAL(clicked(const Client*)), __flightDetails, SLOT(showWindow(const Client*)));
		}
	}
	
	if (!_ap->getInbounds().isEmpty() && _ap->countArrivals()) {
		dMenu->addSeparator();
		QAction* desc = new QAction("Arrivals", this);
		desc->setEnabled(false);
		dMenu->addAction(desc);
		
		for (const Pilot* p: _ap->getInbounds()) {
			if (p->flightStatus != ARRIVED)
				continue;
			ClientDetailsAction* showDetails = new ClientDetailsAction(p, p->callsign, this);
			dMenu->addAction(showDetails);
			connect(showDetails, SIGNAL(clicked(const Client*)), __flightDetails, SLOT(showWindow(const Client*)));
		}
	}
	
	dMenu->exec(mapToGlobal(__lastMousePos));
	delete dMenu;
}

void
MapWidget::__openContextMenu(const Fir* _fir) {
	if (_fir->getStaff().isEmpty())
		return;
	
	QMenu* dMenu = new QMenu(_fir->header.icao, this);
	
	for (const Controller* c: _fir->getStaff()) {
		ClientDetailsAction* showDetails = new ClientDetailsAction(c, c->callsign, this);
		dMenu->addAction(showDetails);
		connect(showDetails, SIGNAL(clicked(const Client*)), __atcDetails, SLOT(showWindow(const Client*)));
	}
	
	dMenu->exec(mapToGlobal(__lastMousePos));
	delete dMenu;
}

void
MapWidget::__loadIcon(const QString& _fName, GLuint& _pix) {
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
MapWidget::__storeSettings() {
	QSettings settings("Vatsinator", "Vatsinator");
	
	settings.beginGroup("CameraSettings");
	
	settings.setValue("zoomFactor", __zoom);
	settings.setValue("cameraPosition", __position);
	
	settings.endGroup();
}

void
MapWidget::__restoreSettings() {
	QSettings settings("Vatsinator", "Vatsinator");
	
	settings.beginGroup("CameraSettings");
	
	__zoom = settings.value("zoomFactor", ZOOM_MINIMUM).toInt();
	__position = settings.value("cameraPosition", QPointF(0.0, 0.0)).toPointF();

	settings.endGroup();
}

void
MapWidget::__produceCircle() {
	__circleCount = 0;
	for (double angle = 0.0; angle <= (2 * PI); angle += 0.1, ++__circleCount)
		{} // count how many vertices we will have
	
	__circle = new GLdouble[__circleCount * 2 + 2];
	unsigned i = 0;
	
	double x, y;
	for (double angle = 0.0; angle <= (2 * PI); angle += 0.1) {
		x = cos(angle);
		y = sin(angle);
		__circle[i++] = x;
		__circle[i++] = y;
	}
}
