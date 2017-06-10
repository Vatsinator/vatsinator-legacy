/*
 * atcdetailswindow.cpp
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
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

#include "atcdetailswindow.h"
#include "ui_atcdetailswindow.h"
#include  "widgetsprivate.h"

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Widgets {

AtcDetailsWindow::AtcDetailsWindow(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AtcDetailsWindow)
{
    ui->setupUi(this);
    
    QFont font = ui->callsign->font();
    font.setPointSize(font.pointSize() + 2);
    ui->callsign->setFont(font);
}

AtcDetailsWindow::~AtcDetailsWindow() {}

void AtcDetailsWindow::setAtc(const Core::Atc* atc)
{
    if (!m_atc.isNull())
        disconnect(m_atc.data());
    
    m_atc = QPointer<const Atc>(atc);
    atcUpdated();
}

void AtcDetailsWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    fixupGeometry(this);
}

void AtcDetailsWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        event->accept();
    } else {
        event->ignore();
    }
}

void AtcDetailsWindow::atcUpdated()
{
    updateCallsign(m_atc->callsign());
    connect(m_atc.data(), &Client::callsignChanged, this, &AtcDetailsWindow::updateCallsign);
    
    updateDescription(m_atc->description());
    connect(m_atc.data(), &Atc::descriptionChanged, this, &AtcDetailsWindow::updateDescription);
    
    updateRealName(m_atc->realName());
    connect(m_atc.data(), &Atc::realNameChanged, this, &AtcDetailsWindow::updateRealName);
    
    updateFrequency(m_atc->frequency());
    connect(m_atc.data(), &Atc::frequencyChanged, this, &AtcDetailsWindow::updateFrequency);
    
    updateRating(m_atc->rating());
    connect(m_atc.data(), &Atc::ratingChanged, this, &AtcDetailsWindow::updateRating);
    
    updateAirport(m_atc->airport());
    connect(m_atc.data(), &Atc::airportChanged, this, &AtcDetailsWindow::updateAirport);
    
    updateServer(m_atc->serverName());
    connect(m_atc.data(), &Atc::serverNameChanged, this, &AtcDetailsWindow::updateServer);
    
    updateOnlineFrom(m_atc->onlineFrom());
    connect(m_atc.data(), &Atc::onlineFromChanged, this, &AtcDetailsWindow::updateOnlineFrom);
    
    updateAtis(m_atc->atis());
    connect(m_atc.data(), &Atc::atisChanged, this, &AtcDetailsWindow::updateAtis);
}

void AtcDetailsWindow::updateCallsign(QString callsign)
{
    ui->callsign->setText(callsign);
    setWindowTitle(callsign);
}

void AtcDetailsWindow::updateDescription(QString description)
{
    ui->description->setText(description);
}

void AtcDetailsWindow::updateRealName(QString realName)
{
    ui->name->setText(realName);
}

void AtcDetailsWindow::updateFrequency(QString frequency)
{
    ui->frequency->setText(frequency);
}

void AtcDetailsWindow::updateRating(Atc::Rating rating)
{
    ui->rating->setText(Atc::ratingStr(rating));
}

void AtcDetailsWindow::updateAirport(Airport *airport)
{
    if (airport)
        ui->airport->setText(airport->representativeName());
    else
        //: not applicable
        ui->airport->setText(tr("N/A"));

    ui->airport->setDisabled(airport == nullptr);
}

void AtcDetailsWindow::updateServer(QString server)
{
    ui->server->setText(server);
}

void AtcDetailsWindow::updateOnlineFrom(QDateTime onlineFrom)
{
    ui->onlineFrom->setText(onlineFrom.toString("dd MM yyyy, hh:mm"));
}

void AtcDetailsWindow::updateAtis(QString atis)
{
    ui->atis->setPlainText(atis);
}

}} /* namespace Vatsinator::Widgets */
