/*
 * metarmanager.cpp
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

#include "metarmanager.h"
#include "filedownloader.h"
#include <QtCore>

namespace Vatsinator { namespace Core {

MetarManager::MetarManager(QObject* parent) :
    QObject(parent),
    m_downloader(new FileDownloader(this))
{
    connect(m_downloader, &FileDownloader::finished, this, &MetarManager::readAllMetars);
}

void MetarManager::setUrl(const QUrl& url)
{
    m_url = url;
    fetchMetars();
}

Metar MetarManager::find(const QString& icao) const
{
    // binary search
    auto it = std::lower_bound(m_metars.cbegin(), m_metars.cend(), icao, [](const Metar& a, const QString& b) {
        return a.icao() < b;
    });
    
    return (*it);
}

void MetarManager::fetchMetars()
{
    m_downloader->fetch(m_url);
}

void MetarManager::readAllMetars()
{
    QString data(m_downloader->data());
    auto rawMetars = data.split(QChar('\n'));
    
    m_metars.clear();
    m_metars.reserve(rawMetars.length());
    
    for (const QString& m: rawMetars) {
        if (m.isEmpty())
            continue;

        m_metars.append(Metar(m));
    }
    
    std::sort(m_metars.begin(), m_metars.end(), [](const Metar& a, const Metar& b) {
        return a.icao() < b.icao();
    });
    
    qDebug() << m_metars.size() << "METARs read";
    
    emit updated();
    QTimer::singleShot(10 * 60 * 1000, Qt::VeryCoarseTimer, this, &MetarManager::fetchMetars);
}

}} /* namespace Vatsinator::Core */
