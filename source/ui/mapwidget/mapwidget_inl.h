/*
 *  mapwidget_inl.h
 *  Copyright (C) 2012  Michał Garapich garrappachc@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/* Some helpful functions */

template< typename T >
inline T absHelper(const T& _v) {
	if (_v < 0)
		return -_v;
	return _v;
}

inline void drawCallsign(const Pilot* _p) {
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.1f);
		glBindTexture(GL_TEXTURE_2D, _p->callsignTip); checkGLErrors(HERE);
		glVertexPointer(2, GL_FLOAT, 0, PILOT_TOOLTIP_VERTICES); checkGLErrors(HERE);
		glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix(); checkGLErrors(HERE);
}

inline void drawCallsign(GLfloat _x, GLfloat _y, const Pilot* _p) {
	glPushMatrix();
		glTranslatef(_x, _y, 0.1f);
		glBindTexture(GL_TEXTURE_2D, _p->callsignTip); checkGLErrors(HERE);
		glVertexPointer(2, GL_FLOAT, 0, PILOT_TOOLTIP_VERTICES); checkGLErrors(HERE);
		glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix(); checkGLErrors(HERE);
}

inline void drawIcaoLabel(const AirportObject* _ap) {
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.1f);
		glBindTexture(GL_TEXTURE_2D, _ap->labelTip); checkGLErrors(HERE);
		glVertexPointer(2, GL_FLOAT, 0, AIRPORT_TOOLTIP_VERTICES); checkGLErrors(HERE);
		glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix(); checkGLErrors(HERE);
}

inline void drawFirLabel(GLfloat _x, GLfloat _y, const Fir& _f) {
	if (_f.icaoTip) {
		glPushMatrix();
			glTranslatef(_x, _y, 0.0f);
			glBindTexture(GL_TEXTURE_2D, _f.icaoTip); checkGLErrors(HERE);
			glVertexPointer(2, GL_FLOAT, 0, FIR_TOOLTIP_VERTICES); checkGLErrors(HERE);
			glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
			glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();checkGLErrors(HERE);
	}
}

inline float
MapWidget::__distanceFromCamera(float _x, float _y) {
	return sqrt(
		pow(_x - __lastMousePosInterpolated.x(), 2) +
		pow(_y - __lastMousePosInterpolated.y(), 2)
	);
}

inline void
MapWidget::__mapCoordinates(float _xFrom, float _yFrom,
		 float& _xTo, float& _yTo) {
	_xTo = (_xFrom / 180 - __position.x()) * __zoom;
	_yTo = (_yFrom / 90 - __position.y()) * __zoom;
}

inline QString
MapWidget::__producePilotToolTip(const Pilot* _p) {
	return (QString)
		"<center>" %
		_p->callsign % "<br><nobr>" %
		_p->realName % " (" % _p->aircraft % ")</nobr><br><nobr>" %
		(_p->route.origin.isEmpty() ? "(unknown)" : (__airports[_p->route.origin]->getData() ?
				_p->route.origin % " " % (QString)__airports[_p->route.origin]->getData()->city :
				_p->route.origin)) %
		" > " %
		(_p->route.destination.isEmpty() ? "(unknown)" : (__airports[_p->route.destination]->getData() ?
				_p->route.destination % " " % (QString)__airports[_p->route.destination]->getData()->city :
				_p->route.destination)) %
		"</nobr><br>" %
		"Ground speed: " % QString::number(_p->groundSpeed) % " kts<br>Altitude: " %
		QString::number(_p->altitude) % " ft</center>";
}

inline QString
MapWidget::__produceAirportToolTip(const AirportObject* _ap) {
	QString text = (QString)"<center>" % (QString)_ap->getData()->icao % "<br><nobr>" %
		(QString)_ap->getData()->name % ", " %
		(QString)_ap->getData()->city % "</nobr>";
	
	for (const Controller* c: _ap->getStaff())
		text.append((QString)"<br><nobr>" %
			c->callsign % " " % c->frequency % " " % c->realName %
			"</nobr>"
		);
	
	int deps = _ap->countDepartures();
	if (deps)
		text.append((QString)"<br>Departures: " % QString::number(deps));
	
	int arrs = _ap->countArrivals();
	if (arrs)
		text.append((QString)"<br>Arrivals: " % QString::number(arrs));
	
	text.append("</center>");
	return text;
}

inline QString
MapWidget::__produceFirToolTip(const Fir* _f) {
	if (_f->name.isEmpty() && _f->getStaff().isEmpty() && _f->getUirStaff().isEmpty())
		return "";
	
	QString text = (QString)"<center>";
	if (!_f->name.isEmpty()) {
		text.append((QString)"<nobr>" % _f->name);
		if (!_f->country.isEmpty())
			text.append((QString)", " % _f->country);
		text.append((QString)"</nobr>");
	}
	
	for (const Controller* c: _f->getStaff())
		text.append((QString)"<br><nobr>" %
			c->callsign % " " % c->frequency % " " % c->realName %
			"</nobr>"
		);
	
	for (const Controller* c: _f->getUirStaff())
		text.append((QString)"<br><nobr>" %
			c->callsign % " " % c->frequency % " " % c->realName %
			"</nobr>"
		);
	
	text.append("</center>");
	return text;
}

