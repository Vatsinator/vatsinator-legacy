/*
 * notam.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QtCore>

#include "notam.h"

Notam::Notam(QString _ident) :
     __ident(qMove(_ident)) {}

Notam::Notam(QString _ident, QString _icao, QString _notam, QString _url,
             QDateTime _from, QDateTime _to, QString _diurnal,
             Notam::CFlag _cflag, Notam::Type _type) :
     __ident(qMove(_ident)),
     __icao(qMove(_icao)),
     __notam(qMove(_notam)),
     __url(qMove(_url)),
     __from(qMove(_from)),
     __to(qMove(_to)),
     __diurnal(qMove(_diurnal)),
     __cflag(_cflag),
     __type(_type) {}

void
Notam::setIcao(const QString& _icao) {
  __icao = _icao;
}

void
Notam::setNotam(const QString& _notam) {
  __notam = _notam;
}

void
Notam::setUrl(const QString& _url) {
  __url = _url;
}

void
Notam::setFrom(const QDateTime& _from) {
  __from = _from;
}

void
Notam::setTo(const QDateTime& _to) {
  __to = _to;
}

void
Notam::setDiurnal(const QString& _diurnal) {
  __diurnal = _diurnal;
}

void
Notam::setCflag(Notam::CFlag _cflag) {
  __cflag = _cflag;
}

void
Notam::setType(Notam::Type _type) {
  __type = _type;
}

bool
Notam::operator <(const Notam& _other) const {
  if (__type == Cancellation && _other.type() != Cancellation)
    return false;
  
  if (_other.type() == Cancellation && __type != Cancellation)
    return true;
  
  if (__cflag == Perm && _other.cflag() != Perm)
    return false;
  
  if (_other.cflag() == Perm && __cflag != Perm)
    return true;
  
  return __from > _other.from();
}

bool
Notam::operator >(const Notam& _other) const {
  if (__type == Cancellation && _other.type() != Cancellation)
    return true;
  
  if (_other.type() == Cancellation && __type != Cancellation)
    return false;
  
  if (__cflag == Perm && _other.cflag() != Perm)
    return true;
  
  if (_other.cflag() == Perm && __cflag != Perm)
    return false;
  
  return __from < _other.from();
}
