/*
 * vatsimstatusdocument.cpp
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#include "vatsimstatusdocument.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

class VatsimStatusDocumentData : public QSharedData {
public:
    QUrl moveTo;
    QString message;
    QUrl metar;
    QList<QUrl> serversListUrls;
    QList<QUrl> dataFileUrls;
};

VatsimStatusDocument::VatsimStatusDocument() :
    d(new VatsimStatusDocumentData) {}

VatsimStatusDocument::VatsimStatusDocument(const VatsimStatusDocument& other) :
    d(other.d) {}

VatsimStatusDocument& VatsimStatusDocument::operator=(const VatsimStatusDocument& other)
{
    d = other.d;
    return *this;
}

VatsimStatusDocument::~VatsimStatusDocument() {}

QUrl VatsimStatusDocument::moveTo() const
{
    return d->moveTo;
}

void VatsimStatusDocument::setMoveTo(const QUrl& moveTo)
{
    d->moveTo = moveTo;
}

QString VatsimStatusDocument::message() const
{
    return d->message;
}

void VatsimStatusDocument::setMessage(const QString& message)
{
    d->message = message;
}

QUrl VatsimStatusDocument::metar() const
{
    return d->metar;
}

void VatsimStatusDocument::setMetar(const QUrl& metar)
{
    d->metar = metar;
}

QList<QUrl> VatsimStatusDocument::serversListUrls() const
{
    return d->serversListUrls;
}

void VatsimStatusDocument::setServersListUrls(QList<QUrl> serversListUrls)
{
    d->serversListUrls = serversListUrls;
}

QList<QUrl> VatsimStatusDocument::dataFileUrls() const
{
    return d->dataFileUrls;
}

void VatsimStatusDocument::setDataFileUrls(QList<QUrl> dataFileUrls)
{
    d->dataFileUrls = dataFileUrls;
}

bool VatsimStatusDocument::isValid() const
{
    return dataFileUrls().length() > 0 && !metar().isEmpty();
}

}} /* namespace Vatsinator::Core */

static void registerMetatype()
{
    qRegisterMetaType<Vatsinator::Core::VatsimStatusDocument>();
}
Q_COREAPP_STARTUP_FUNCTION(registerMetatype)
