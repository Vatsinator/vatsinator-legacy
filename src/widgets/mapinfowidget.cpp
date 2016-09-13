/*
 * mapinfowidget.cpp
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

#include "mapinfowidget.h"
#include "ui_mapinfowidget.h"
#include <QtWidgets>

namespace Vatsinator { namespace Widgets {

MapInfoWidget::MapInfoWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MapInfoWidget)
{
    ui->setupUi(this);
    
    QFont font;
    font.setBold(true);
    font.setPointSize(font.pointSize() - 1);
    
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(65, 65, 65));
    
    auto effect = [this]() -> QGraphicsDropShadowEffect* {
        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
        effect->setBlurRadius(5.0);
        effect->setOffset(QPointF(0.0, 0.0));
        effect->setColor(Qt::white); // white shadow gives as a gentle outline
        return effect;
    };
    
    ui->status->setFont(font);
    ui->updated->setFont(font);
    
    ui->status->setPalette(palette);
    ui->updated->setPalette(palette);
    
    ui->status->setGraphicsEffect(effect());
    ui->updated->setGraphicsEffect(effect());
    
    updateStatusLabel();
}

MapInfoWidget::~MapInfoWidget() {}

void MapInfoWidget::setUpdated(const QDateTime& updated)
{
    m_updated = updated;
    
    if (m_updated.isNull())
        ui->updated->setText(QString());
    else
        ui->updated->setText(tr("Last update: %1 UTC").arg(m_updated.toString("dd MMM yyyy, hh:mm")));
}

void MapInfoWidget::setClients(int clients)
{
    m_clients = clients;
    updateStatusLabel();
}

void MapInfoWidget::setPilots(int pilots)
{
    m_pilots = pilots;
    updateStatusLabel();
}

void MapInfoWidget::setAtcs(int atcs)
{
    m_atcs = atcs;
    updateStatusLabel();
}

void MapInfoWidget::setObservers(int observers)
{
    m_observers = observers;
    updateStatusLabel();
}

void MapInfoWidget::updateStatusLabel()
{
    if (m_clients == 0) {
        ui->status->setText(QString());
        return;
    }
    
    QString clients = tr("%n client(s)", "", m_clients);
    QString pilots = tr("%n pilot(s)", "", m_pilots);
    QString atcs = tr("%n ATC(s)", "", m_atcs);
    QString observers = tr("%n observer(s)", "", m_observers);
    ui->status->setText(QStringLiteral("%1 (%2, %3, %4)").arg(clients, pilots, atcs, observers));
}


}} /* namespace Vatsinator::Widgets */
