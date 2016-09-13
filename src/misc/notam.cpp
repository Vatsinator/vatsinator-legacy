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

#include "notam.h"
#include <QtCore>

namespace Vatsinator { namespace Misc {

class NotamData : public QSharedData {
public:
    NotamData() = default;
    NotamData(const QString& ident) : ident(ident) {}
    
    QString ident;
    QString icao;
    QString notam;
    QString url;
    QDateTime from;
    QDateTime to;
    QString diurnal;
    Notam::CFlag cflag;
    Notam::Type type;
};

Notam::Notam() : d(new NotamData) {}

Notam::Notam(const QString& ident) :
    d(new NotamData(ident)) {}

Notam::Notam(const Notam& orig) :
    d(orig.d) {}

Notam::~Notam() {}

Notam& Notam::operator=(const Notam& other)
{
    d = other.d;
    return *this;
}

QString Notam::ident() const
{
    return d->ident;
}

QString Notam::icao() const
{
    return d->icao;
}

void Notam::setIcao(const QString& icao)
{
    d->icao = icao;
}

QString Notam::notam() const
{
    return d->notam;
}

void Notam::setNotam(const QString& notam)
{
    d->notam = notam;
}

QString Notam::url() const
{
    return d->url;
}

void Notam::setUrl(const QString& url)
{
    d->url = url;
}

QDateTime Notam::from() const
{
    return d->from;
}

void Notam::setFrom(const QDateTime& from)
{
    d->from = from;
}

QDateTime Notam::to() const
{
    return d->to;
}

void Notam::setTo(const QDateTime& to)
{
    d->to = to;
}

QString Notam::diurnal() const
{
    return d->diurnal;
}

void Notam::setDiurnal(const QString& diurnal)
{
    d->diurnal = diurnal;
}

Notam::CFlag Notam::cflag() const
{
    return d->cflag;
}

void Notam::setCflag(Notam::CFlag cflag)
{
    d->cflag = cflag;
}

Notam::Type Notam::type() const
{
    return d->type;
}

void Notam::setType(Notam::Type type)
{
    d->type = type;
}

}} /* namespace Vatsinator::Misc */
