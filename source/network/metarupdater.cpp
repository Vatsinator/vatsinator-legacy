/*
 * metarupdater.cpp
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

#include <QtCore>

#include "models/metarlistmodel.h"
#include "models/roles.h"
#include "network/plaintextdownloader.h"
#include "vatsimdata/metar.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "metarupdater.h"

static Q_CONSTEXPR int UpdateInterval = 5 * 1000 * 60; // 5 minutes

MetarUpdater::MetarUpdater(MetarListModel* model, QObject* parent) :
    QObject (parent),
    __metars(model),
    __downloader(new PlainTextDownloader(this))
{
    connect(__downloader, &PlainTextDownloader::finished, this, &MetarUpdater::__readMetars);
    startTimer(UpdateInterval, Qt::VeryCoarseTimer);
}

Metar*
MetarUpdater::fetch(const QString& request)
{
    QUrl vatsimMetars = vApp()->vatsimDataHandler()->metar();
    if (vatsimMetars.isEmpty()) {
        qDebug("Vatsim status not read yet; postponing request");
        __requests.enqueue(request);
    } else {
        QUrlQuery query(QStringLiteral("id=%1").arg(request.toLower()));
        QUrl url(vatsimMetars);
        url.setQuery(query);
        __downloader->fetch(url);
        __requests.enqueue(request);
    }
    
    if (!VatsimDataHandler::isValidIcao(request))
        return nullptr;
    
    auto matches = vApp()->metarUpdater()->model()->match(vApp()->metarUpdater()->model()->index(0), MetarRole, request, 1);
    if (matches.length() > 0) {
        Metar* metar = matches.first().data(MetarRole).value<Metar*>();
        return metar;
    } else {
        Metar* metar = new Metar(request);
        __metars->addMetar(metar);
        return metar;
    }
}

void
MetarUpdater::update()
{
    for (int i = 0; i < __metars->rowCount(); ++i) {
        auto index = __metars->data(__metars->index(i), MetarRole);
        
        if (index.isValid()) {
            Q_ASSERT(index.canConvert<Metar*>());
            Metar* m = index.value<Metar*>();
            fetch(m->icao());
        }
    }
}

void
MetarUpdater::timerEvent(QTimerEvent* event)
{
    update();
    Q_UNUSED(event);
}

void
MetarUpdater::__update(const QString& metar)
{
    QString icao = metar.left(4);
    Q_ASSERT(VatsimDataHandler::isValidIcao(icao));
    
    auto matches = __metars->match(__metars->index(0), MetarRole, icao, 1);
    if (matches.length() > 0) {
        Metar* m = matches.first().data(MetarRole).value<Metar*>();
        m->setMetar(metar);
    } else {
        Metar* m = new Metar(icao, metar);
        __metars->addMetar(m);
    }
}

void
MetarUpdater::__readMetars()
{
    // How to recognize unavailable metars.
    // This value represents the response returned by vatsim server
    // in case given METAR could not be found.
    static const QString NoMetarText = QStringLiteral("No METAR available");
    
    QString response = __downloader->data().simplified();
    if (response.isEmpty())
        return;
    
    if (response.contains(NoMetarText)) {
        __requests.dequeue();
        return;
    }
    
    QString metar;
    QStringList words = response.split(' ');
    
    for (const auto& w : words) {
        if (w.length() == 4 && w.startsWith(__requests.head(), Qt::CaseInsensitive)) {
            if (!metar.isEmpty())
                __update(metar);
                
            metar.clear();
        }
        
        metar.append(w).append(" ");
    }
    
    if (!metar.isEmpty())
        __update(metar);
        
    __requests.dequeue();
}


