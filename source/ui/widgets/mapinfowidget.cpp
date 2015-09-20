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
    
    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::white);
    
    __labelUpdated = new QLabel;
    __labelUpdated->setFont(font);
    __labelUpdated->setPalette(palette);
    __labelUpdated->setGraphicsEffect(__prepareEffect());
    
    __labelUpdating = new QLabel(tr("Updating..."));
    __labelUpdating->setFont(font);
    __labelUpdating->setPalette(palette);
    __labelUpdating->setGraphicsEffect(__prepareEffect());
    
    __labelStatus = new QLabel;
    __labelStatus->setAlignment(Qt::AlignRight);
    __labelStatus->setFont(font);
    __labelStatus->setPalette(palette);
    __labelStatus->setGraphicsEffect(__prepareEffect());
    
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(__labelUpdated);
    layout->addWidget(__labelUpdating);
    layout->addWidget(__labelStatus);
    setLayout(layout);
    
    setUpdatedVisible(true);
}

void
MapInfoWidget::setPosition(const LonLat& position)
{
    __position = position;
//     __labelPosition->setText(QStringLiteral("%1 %2 %3 %4").arg(
//         position.latitude() > 0 ? "N" : "S",
//         QString::number(qAbs(position.latitude()), 'g', 6),
//         position.longitude() < 0 ? "W" : "E",
//         QString::number(qAbs(position.longitude()), 'g', 6)
//     ));
    /* Not used */
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

void
MapInfoWidget::setUpdatedVisible(bool updatedVisible)
{
    __updatedVisible = updatedVisible;
    __labelUpdated->setVisible(updatedVisible);
    __labelUpdating->setVisible(!updatedVisible);
}

void
MapInfoWidget::setClients(int clients)
{
    __clients = clients;
    __updateStatusLabel();
}

void
MapInfoWidget::setPilots(int pilots)
{
    __pilots = pilots;
    __updateStatusLabel();
}

void
MapInfoWidget::setAtcs(int atcs)
{
    __atcs = atcs;
    __updateStatusLabel();
}

void
MapInfoWidget::setObservers(int observers)
{
    __observers = observers;
    __updateStatusLabel();
}

QGraphicsDropShadowEffect*
MapInfoWidget::__prepareEffect()
{
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(15.0);
    effect->setOffset(QPointF(0.0, 0.0));
    effect->setColor(Qt::black);
    return effect;
}

void
MapInfoWidget::__updateStatusLabel()
{
    QString clients = tr("%n client(s)", "", __clients);
    QString pilots = tr("%n pilot(s)", "", __pilots);
    QString atcs = tr("%n ATC(s)", "", __atcs);
    QString observers = tr("%n observer(s)", "", __observers);
    __labelStatus->setText(QStringLiteral("%1 (%2, %3, %4)").arg(clients, pilots, atcs, observers));
}
