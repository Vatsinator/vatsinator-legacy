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

Notam::Notam(QString ident) :
     __ident(qMove(ident)) {}

Notam::Notam(QString ident, QString icao, QString notam, QString url,
             QDateTime from, QDateTime to, QString diurnal,
             Notam::CFlag cflag, Notam::Type type) :
     __ident(qMove(ident)),
     __icao(qMove(icao)),
     __notam(qMove(notam)),
     __url(qMove(url)),
     __from(qMove(from)),
     __to(qMove(to)),
     __diurnal(qMove(diurnal)),
     __cflag(cflag),
     __type(type) {}

void
Notam::setIcao(const QString& icao) {
  __icao = icao;
}

void
Notam::setNotam(const QString& notam) {
  __notam = notam;
}

void
Notam::setUrl(const QString& url) {
  __url = url;
}

void
Notam::setFrom(const QDateTime& from) {
  __from = from;
}

void
Notam::setTo(const QDateTime& to) {
  __to = to;
}

void
Notam::setDiurnal(const QString& diurnal) {
  __diurnal = diurnal;
}

void
Notam::setCflag(Notam::CFlag cflag) {
  __cflag = cflag;
}

void
Notam::setType(Notam::Type type) {
  __type = type;
}

bool
Notam::operator <(const Notam& other) const {
  if (__type == Cancellation && other.type() != Cancellation)
    return false;
  
  if (other.type() == Cancellation && __type != Cancellation)
    return true;
  
  if (__cflag == Perm && other.cflag() != Perm)
    return false;
  
  if (other.cflag() == Perm && __cflag != Perm)
    return true;
  
  return __from > other.from();
}

bool
Notam::operator >(const Notam& other) const {
  if (__type == Cancellation && other.type() != Cancellation)
    return true;
  
  if (other.type() == Cancellation && __type != Cancellation)
    return false;
  
  if (__cflag == Perm && other.cflag() != Perm)
    return true;
  
  if (other.cflag() == Perm && __cflag != Perm)
    return false;
  
  return __from < other.from();
}
