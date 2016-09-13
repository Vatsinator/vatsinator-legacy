/*
 * uirlistreader.cpp
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
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

#include "uirlistreader.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

UirListReader::UirListReader(QObject* parent) :
    ResourceFile(QStringLiteral("data/uirs.json"), parent)
{
    readData();
    connect(this, &FileReader::fileRead, this, &UirListReader::readData);
}

std::pair<bool, UirListReader::UirData> UirListReader::find(const QString& icao)
{
    if (m_uirs.contains(icao))
        return std::make_pair(true, m_uirs[icao]);
    else
        return std::make_pair(false, std::make_tuple(QString(), QString(), QList<QString>()));
}

void UirListReader::readData()
{
    QJsonDocument document = QJsonDocument::fromJson(data());
    Q_ASSERT(document.isArray());
    auto array = document.array();
    for (auto v: array) {
        auto value = v.toObject();
        QString icao = value.value("icao").toString();
        QString name = value.value("name").toString();
        QList<QString> firs;
        auto a2 = value.value("firs").toArray();
        for (auto v2: a2)
            firs.append(v2.toString());

        m_uirs.insert(icao, std::make_tuple(icao, name, firs));
    }

    qDebug("UirListReader: read %d UIRs", m_uirs.size());
}

}} /* namespace Vatsinator::Core */
