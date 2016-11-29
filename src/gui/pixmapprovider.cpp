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

#include "modelmatcher.h"
#include "pixmapprovider.h"
#include <QtGui>

namespace Vatsinator { namespace Gui {

QString generalizedDensity()
{
    static QString gd;
    if (gd.isEmpty()) {
        qreal dp = qApp->primaryScreen()->devicePixelRatio();
        if (dp <= 1.0)
            gd = QStringLiteral("mdpi");
        else if (dp <= 1.5)
            gd = QStringLiteral("hdpi");
        else if (dp <= 2.0)
            gd = QStringLiteral("xhdpi");
        else if (dp <= 3.0)
            gd = QStringLiteral("xxhdpi");
        else
            gd = QStringLiteral("xxxhdpi");
    }
    return gd;
}

PixmapProvider::PixmapProvider(QObject* parent) :
    QObject(parent),
    m_defaultModelMatcher(new ModelMatcher(this)) {}

PixmapProvider::~PixmapProvider() {}

QPixmap PixmapProvider::pixmapForModel(const QString& modelString) const
{
    QString model = matchModel(modelString);
    QPixmap px;
    if (!QPixmapCache::find(model, &px)) {
       /**
        * In order to provide maximum flexibility, the model pixmaps
        * are white with black outline and here they are colorized.
        * The result itself is then stored in the pixmap cache.
        */
        QString path = ":/pixmaps/" + generalizedDensity() + "/" + model + ".png";
        QImage image(path);
        Q_ASSERT(!image.isNull());
        
        QImage colorized = modelColorized(image, QColor(238, 220, 122));
        px.convertFromImage(colorized);
        px.setDevicePixelRatio(qApp->primaryScreen()->devicePixelRatio());
        
        QPixmapCache::insert(model, px);
    }
    
    return px;
}

QPixmap PixmapProvider::pixmapForAirport(bool staffed) const
{
    static const QString AirportActivePixmap = QStringLiteral(":/pixmaps/%1/airport.png");
    static const QString AirportActiveStaffedPixmap = QStringLiteral(":/pixmaps/%1/airport_staffed.png");
    
    QString key = staffed ? QStringLiteral("airport_staffed") : QStringLiteral("airport");
    QPixmap px;
    if (!QPixmapCache::find(key, &px)) {
        QString pxSrc = staffed ? AirportActiveStaffedPixmap : AirportActivePixmap;
        px.load(pxSrc.arg(generalizedDensity()));
        px.setDevicePixelRatio(qApp->primaryScreen()->devicePixelRatio());
        
        QPixmapCache::insert(key, px);
    }
    
    return px;
}

QPixmap PixmapProvider::flightTooltipBackground() const
{
    static const QString Key = QStringLiteral("flight_tooltip_background");

    QPixmap px;
    if (!QPixmapCache::find(Key, &px)) {
        QFont font;
        QFontMetrics metrics(font);

        /* Tooltip shadow offsets */
        constexpr int ShadowWidth = 0;
        constexpr int ShadowHeight = 2;

        QSize baseSize(9 * metrics.averageCharWidth(), metrics.height());
        QSize arrowSize(16 * qApp->primaryScreen()->devicePixelRatio(), 8 * qApp->primaryScreen()->devicePixelRatio());
        QSize size(baseSize.width() + ShadowWidth, baseSize.height() + ShadowHeight + arrowSize.height());

        /* Pixmap is large enough to contain the rounded rectangle and the shadow */
        px = QPixmap(size);
        px.fill(Qt::transparent);
        QPainter p(&px);

        qreal radius = 3.0 * qApp->primaryScreen()->devicePixelRatio();

        /* Design the tooltip */
        QPainterPath path;
        path.addRoundedRect(QRect(QPoint(0, 0), baseSize), radius, radius);
        int o = arrowSize.width() / 2;
        path.moveTo(baseSize.width() / 2 - o, baseSize.height());
        path.lineTo(baseSize.width() / 2, baseSize.height() + arrowSize.height());
        path.lineTo(baseSize.width() / 2 + o, baseSize.height());
        path.lineTo(baseSize.width() / 2 - o, baseSize.height());

        /* First, draw the shadow */
        QLinearGradient shadow(QPoint(0, 0), QPoint(0, baseSize.height()));
        shadow.setColorAt(0.0, QColor(145, 145, 145, 105));
        shadow.setColorAt(1.0, QColor(175, 175, 175, 75));
        p.fillPath(path.translated(ShadowWidth, ShadowHeight), QBrush(shadow));

        /* Then draw the background itself */
        QRadialGradient gradient(QPointF(baseSize.width() / 2, 0.0), baseSize.width() / 2);
        gradient.setColorAt(0.0, QColor(250, 250, 250));
        gradient.setColorAt(1.0, QColor(235, 235, 235));

        p.fillPath(path, QBrush(gradient));
        p.end();

        QPixmapCache::insert(Key, px);
    }

    return px;
}

QPixmap PixmapProvider::airportLabelBackground(const QFont& font) const
{
    static const QString Key = QStringLiteral("airport_label_background");

    QPixmap px;
    if (!QPixmapCache::find(Key, &px)) {
        QFontMetrics metrics(font);

        QSize size(6 * metrics.averageCharWidth(), metrics.height());

        px = QPixmap(size);
        px.fill(Qt::transparent);
        QPainter p(&px);

        qreal radius = 3.0 * qApp->primaryScreen()->devicePixelRatio();

        QPainterPath path;
        path.addRoundedRect(px.rect(), radius, radius);

        p.fillPath(path, QBrush(QColor(75, 75, 75, 200)));
        p.end();

        QPixmapCache::insert(Key, px);
    }

    return px;
}

void PixmapProvider::setModelMatcher(const ModelMatcher* modelMatcher)
{
    m_modelMatcher = modelMatcher;
}

QString PixmapProvider::matchModel(const QString& modelString) const
{
    if (!m_modelMatcher)
        return m_defaultModelMatcher->match(modelString);
    else
        return m_modelMatcher->match(modelString);
}

QImage PixmapProvider::modelColorized(const QImage& image, const QColor& color) const
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
        
        int nr = 255 - *pr + r; // new red
        int ng = 255 - *pg + g; // new green
        int nb = 255 - *pb + b; // new blue
        
        *pr = qBound(0, 255 - nr, 255);
        *pg = qBound(0, 255 - ng, 255);
        *pb = qBound(0, 255 - nb, 255);
    }
    
    return colorized;
}

}} /* namespace Vatsinator::Gui */
