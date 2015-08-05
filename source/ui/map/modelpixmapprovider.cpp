/*
 * modelpixmapprovider.cpp
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
#include "ui/map/mapconfig.h"

#include "modelpixmapprovider.h"

ModelPixmapProvider::ModelPixmapProvider(QObject* parent) : QObject (parent)
{
    __readMappings();
}

QPixmap
ModelPixmapProvider::pixmapForModel(const QString& modelString)
{
    QString model = __matchModel(modelString);
    
    QPixmap px;
    if (!QPixmapCache::find(model, &px)) {
        px = __prepareModelPixmap(model);
    }
    
    return px;
}

void
ModelPixmapProvider::__readMappings()
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
ModelPixmapProvider::__matchModel(const QString modelString)
{
    if (modelString.isEmpty())
        return "B737";
    
    QRegExp rx("(\\w{3,})");
    if (rx.indexIn(modelString) == -1) {
        qWarning("No model match for %s ", qPrintable(modelString));
        return "B737";
    }
    
    return __mappings.value(rx.cap(1), "B737");
}

QPixmap
ModelPixmapProvider::__prepareModelPixmap(const QString& model)
{
    /**
     * In order to provide maximum flexibility, the model pixmaps
     * are white with black outline and here they are colorized.
     * The result itself is then stored in the pixmap cache.
     */
    
    QPixmap px;
    if (QPixmapCache::find(model, &px))
        return px;
    
    QString path = ":/pixmaps/" + MapConfig::generalizedDensity() + "/" + model + ".png";
    QImage image(path);
    Q_ASSERT(!image.isNull());
    
    QImage colorized = __modelColorized(image, QColor(238, 220, 122));
    px.convertFromImage(colorized);
    
    QPixmapCache::insert(model, px);
    return px;
}

QImage
ModelPixmapProvider::__modelColorized(const QImage& image, const QColor& color)
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
