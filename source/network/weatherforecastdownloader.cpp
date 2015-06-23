/*
 * weatherforecastdownloader.cpp
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
#include "plugins/weatherforecastinterface.h"
#include "plugins/weatherforecastreply.h"
#include "plugins/weatherforecastrequest.h"

#include "weatherforecastdownloader.h"

QList<WeatherForecastInterface*> WeatherForecastDownloader::__providers;
bool WeatherForecastDownloader::__initialized = false;

WeatherForecastDownloader::WeatherForecastDownloader(QObject* parent):
    QObject(parent),
    __request(nullptr)
{

    if (!__initialized)
        __initializeProviders();
}

void
WeatherForecastDownloader::download(WeatherForecastRequest* request)
{
    Q_ASSERT(!__request);
    
    __currentProvider = __providers.begin();
    __request = request;
    __useCurrentProvider();
}

void
WeatherForecastDownloader::__useCurrentProvider()
{
    if (__currentProvider == __providers.end()) {
        WeatherForecastReply* reply = new WeatherForecastReply(__request, this);
        reply->setError(WeatherForecastReply::NetworkError);
        emit finished(reply);
        __request = nullptr;
    }
    
    WeatherForecastInterface* provider = *__currentProvider;
    WeatherForecastReply* reply = provider->fetch(__request);
    connect(reply, &WeatherForecastReply::finished, this, &WeatherForecastDownloader::__replyFinished);
    
}

void
WeatherForecastDownloader::__replyFinished()
{
    WeatherForecastReply* reply = qobject_cast<WeatherForecastReply*>(sender());
    Q_ASSERT(reply);
    
    switch (reply->error()) {
        case WeatherForecastReply::NotFoundError:
        case WeatherForecastReply::NetworkError:
            ++__currentProvider;
            __useCurrentProvider();
            break;
            
        case WeatherForecastReply::NoError:
            emit finished(reply);
            __request = nullptr;
            break;
    }
}

void
WeatherForecastDownloader::__initializeProviders()
{
    auto plugins = QPluginLoader::staticPlugins();
    
    for (auto& p : plugins) {
        QJsonObject pluginData = p.metaData();
        
        if (!pluginData["IID"].isString())
            continue;
            
        QString iid = pluginData["IID"].toString();
        
        if (iid != "org.eu.vatsinator.Vatsinator.WeatherForecastInterface")
            continue;
            
        WeatherForecastInterface* i = qobject_cast<WeatherForecastInterface*>(p.instance());
        
        if (i)
            __providers << i;
    }
}
