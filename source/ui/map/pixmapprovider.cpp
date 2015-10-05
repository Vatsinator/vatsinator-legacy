/*
 * pixmapprovider.cpp
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

#include <QtGui>

#include "storage/filemanager.h"
#include "storage/settingsmanager.h"
#include "ui/map/mapconfig.h"
#include "ui/userinterface.h"
#include "vatsinatorapplication.h"

#include "pixmapprovider.h"


constexpr auto AirportLabelBackgroundKey = "airport_label_background";
constexpr auto FlightLabelBackgroundKey = "flight_label_background";
constexpr auto ModelFallback = "B737";


PixmapProvider::PixmapProvider(QObject* parent) : QObject (parent)
{
    connect(vApp()->settingsManager(), &SettingsManager::settingsChanged, [this]() {
        QPixmapCache::remove(AirportLabelBackgroundKey);
    });
    
    __readModelMappings();
}

QPixmap
PixmapProvider::backgroundForAirportLabel()
{
    QPixmap px;
    if (!QPixmapCache::find(AirportLabelBackgroundKey, &px)) {
        QFont font = SettingsManager::get("map.airport_font").value<QFont>();
        QFontMetrics metrics(font);
        
        int width = 4 * metrics.maxWidth(); // 4 letters for ICAO code
        int height = metrics.height();
        px = QPixmap(width, height);
        
        px.fill(Qt::transparent);
        
        qreal radius = 3.0 * UserInterface::dp();
        
        QPainterPath path;
        path.addRoundedRect(px.rect(), radius, radius);
        
        QPainter p(&px);
        p.fillPath(path, QBrush(QColor(75, 75, 75, 200)));
        p.end();
        
        QPixmapCache::insert(AirportLabelBackgroundKey, px);
    }
    
    return px;
}

QPixmap
PixmapProvider::backgroundForFlightLabel(QRect* targetRect)
{
    QPixmap px;
    if (!QPixmapCache::find(FlightLabelBackgroundKey, &px)) {
        QFont font = SettingsManager::get("map.pilot_font").value<QFont>();
        QFontMetrics metrics(font);
        
        /* Tooltip shadow offsets */
        constexpr int ShadowWidth = 0;
        constexpr int ShadowHeight = 2;
        
        int width = 5 * metrics.maxWidth(); // 7 is usually the longest callsign
        int height = metrics.height();
        int arrowHeight = 8 * UserInterface::dp();
        int arrowWidth = 16 * UserInterface::dp();
        
        /* Pixmap is large enough to contain the rounded rectangle and the shadow */
        px = QPixmap(width + ShadowWidth, height + arrowHeight + ShadowHeight);
        
        px.fill(Qt::transparent);
        QPainter p(&px);
        
        qreal radius = 3.0 * UserInterface::dp();
        
        /* Design the tooltip */
        QPainterPath path;
        path.addRoundedRect(QRect(QPoint(0, 0), QSize(width, height)), radius, radius);
        int o = arrowWidth / 2;
        path.moveTo(width / 2 - o, height);
        path.lineTo(width / 2, height + arrowHeight);
        path.lineTo(width / 2 + o, height);
        path.lineTo(width / 2 - o, height);
        
        /* First, draw the shadow */
        QLinearGradient shadow(QPoint(0, 0), QPoint(0, height));
        shadow.setColorAt(0.0, QColor(145, 145, 145, 105));
        shadow.setColorAt(1.0, QColor(175, 175, 175, 75));
        
        p.fillPath(path.translated(ShadowWidth, ShadowHeight), QBrush(shadow));
        
        QRadialGradient gradient(QPointF(width / 2, 0.0), width / 2);
        gradient.setColorAt(0.0, QColor(250, 250, 250));
        gradient.setColorAt(1.0, QColor(235, 235, 235));
        
        p.fillPath(path, QBrush(gradient));
        p.end();
        
        QPixmapCache::insert(FlightLabelBackgroundKey, px);
        __flightLabelRect = QRect(QPoint(0, 0), QSize(width, height));
    }
    
    if (targetRect) {
        Q_ASSERT(!__flightLabelRect.isNull());
        *targetRect = __flightLabelRect;
    }
    
    return px;
}

QPixmap
PixmapProvider::pixmapForModel (const QString& modelString)
{
    QString model = __matchModel(modelString);
    
    QPixmap px;
    if (!QPixmapCache::find(model, &px)) {
        /**
        * In order to provide maximum flexibility, the model pixmaps
        * are white with black outline and here they are colorized.
        * The result itself is then stored in the pixmap cache.
        */
        
        QString path = ":/pixmaps/" + MapConfig::generalizedDensity() + "/" + model + ".png";
        QImage image(path);
        Q_ASSERT(!image.isNull());
        
        QImage colorized = __modelColorized(image, QColor(238, 220, 122));
        px.convertFromImage(colorized);
        
        QPixmapCache::insert(model, px);
    }
    
    return px;
}

void
PixmapProvider::__readModelMappings()
{
    QFile file(FileManager::path("data/model"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Could not open %s for reading", qPrintable(file.fileName()));
        return;
    }
    
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning("VatsimDataHandler: the following error occured parsing %s: %s",
                 qPrintable(file.fileName()), qPrintable(error.errorString()));
        return;
    }
    
    Q_ASSERT(document.isObject());
    QJsonObject object = document.object();
    
    QStringList keys = object.keys();
    std::for_each(keys.begin(), keys.end(), [this, &object](const QString& key) {
        QJsonValue value = object.value(key);
        Q_ASSERT(value.isArray());;
        QJsonArray array = value.toArray();
        
        std::for_each(array.begin(), array.end(), [this, &key](const QJsonValue& value) {
            Q_ASSERT(value.isString());
            QString mapping = value.toString();
            __mappings.insert(mapping, key);
        });
    });
    
    file.close();
}

QString
PixmapProvider::__matchModel(const QString& modelString)
{
    if (modelString.isEmpty())
        return ModelFallback;
    
    QRegExp rx("(\\w{3,})");
    if (rx.indexIn(modelString) == -1) {
        qWarning("No model match for %s ", qPrintable(modelString));
        return ModelFallback;
    }
    
    return __mappings.value(rx.cap(1), ModelFallback);
}

QImage
PixmapProvider::__modelColorized(const QImage& image, const QColor& color)
{
    /**
     * I tried using Qt's alpha masks here and all that stuff, but
     * it didn't end up well. So I came up with my own solution.
     */
    int r, g, b;
    color.getRgb(&r, &g, &b);
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
    
    QImage colorized = image.convertToFormat(QImage::Format_ARGB32);
    uchar* data = colorized.bits();
    for (int i = 0; i < colorized.byteCount() / 4; ++i) {
        // data stored in AARRGGBB format, little-endian
        uchar* pb = data + (i * 4);
        uchar* pg = data + (i * 4 + 1);
        uchar* pr = data + (i * 4 + 2);
//         uchar* pa = data + (i * 4 + 3);
        
        int nr = 255 - *pr + r; // new red
        int ng = 255 - *pg + g; // new green
        int nb = 255 - *pb + b; // new blue
        
        *pr = qBound(0, 255 - nr, 255);
        *pg = qBound(0, 255 - ng, 255);
        *pb = qBound(0, 255 - nb, 255);
    }
    
    return colorized;
}
