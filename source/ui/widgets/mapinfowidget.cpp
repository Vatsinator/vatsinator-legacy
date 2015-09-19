/*
 * mapinfowidget.cpp
 * Copyright (C) 2015  Michal Garapich michal@garapich.pl
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

#include <QtWidgets>

#include "mapinfowidget.h"

MapInfoWidget::MapInfoWidget(QWidget* parent) : QWidget(parent)
{
    QFont font;
    font.setBold(true);
    font.setPointSize(font.pointSize() - 1);
    
    __labelPosition = new QLabel;
    __labelPosition->setAlignment(Qt::AlignRight);
    __labelPosition->setFont(font);
    
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(2.0);
    effect->setOffset(QPointF(1.0, 1.0));
    __labelPosition->setGraphicsEffect(effect);
    
    __labelUpdated = new QLabel;
    __labelUpdated->setFont(font);
    
    effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(2.0);
    effect->setOffset(QPointF(1.0, 1.0));
    __labelUpdated->setGraphicsEffect(effect);
    
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(__labelUpdated);
    layout->addWidget(__labelPosition);
    setLayout(layout);
}

void
MapInfoWidget::setPosition(const LonLat& position)
{
    __position = position;
    __labelPosition->setText(QStringLiteral("%1 %2 %3 %4").arg(
        position.latitude() > 0 ? "N" : "S",
        QString::number(qAbs(position.latitude()), 'g', 6),
        position.longitude() < 0 ? "W" : "E",
        QString::number(qAbs(position.longitude()), 'g', 6)
    ));
    update();
}

void
MapInfoWidget::setUpdated(const QDateTime& updated)
{
    __updated = updated;
    
    if (updated.isNull())
        __labelUpdated->setText(QString());
    else
        __labelUpdated->setText(tr("Last update: %1 UTC").arg(updated.toString("dd MMM yyyy, hh:mm")));
}
