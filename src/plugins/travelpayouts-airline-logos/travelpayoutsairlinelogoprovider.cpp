/*
 * travelpayoutsairlinelogoprovider.cpp
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

#include "travelpayoutsairlinelogoprovider.h"
#include "core/filedownloader.h"
#include <misc/filecache.h>
#include <QtGui>

using namespace Vatsinator::Core;
using namespace Vatsinator::Misc;

static constexpr auto UrlTemplate = "http://pics.avs.io/%1/%2/%3%4.png";


namespace {

QString cacheKey(const QString& iata, const QSize& size)
{
    return QStringLiteral("travelpayouts-airline-logos/%1x%2/%3.png")
        .arg(QString::number(size.width()),
             QString::number(size.height()),
             iata);
}

}


TravelpayoutsAirlineLogoProvider::TravelpayoutsAirlineLogoProvider(QObject* parent) :
    QObject(parent) {}

const AirlineLogoReply* TravelpayoutsAirlineLogoProvider::fetchLogo(const Airline& airline,
                                                                    const QSize& prefferedSize)
{
    AirlineLogoReply* reply = new AirlineLogoReply(airline, this);
    if (airline.iata().isEmpty()) { // travelpayouts accept only IATA codes
        reply->setFinished(true);
        return reply;
    }
    
    QString cachePath = cacheKey(airline.iata(), prefferedSize);
    
    if (FileCache::exists(cachePath)) {
        QPixmap px(FileCache::path(cachePath));
        if (!px.isNull()) {
            reply->setLogo(px);
            reply->setFinished(true);
            return reply;
        }
    }
    
    FileDownloader* fd = new FileDownloader(this);
    connect(fd, &FileDownloader::finished, [=]() {
        FileCache::storeInCache(cachePath, fd->data());
        
        QPixmap px(FileCache::path(cachePath));
        if (!px.isNull())
            reply->setLogo(px);
        
        reply->setFinished(true);
        fd->deleteLater();
    });
    
    QString at2x;
    if (qApp->primaryScreen()->devicePixelRatio() >= 2.0)
        at2x = QStringLiteral("@2x");
    
    QUrl url(QString(UrlTemplate).arg(QString::number(prefferedSize.width()),
                                      QString::number(prefferedSize.height()),
                                      airline.iata(), at2x));
    fd->fetch(url);
    
    return reply;
}
