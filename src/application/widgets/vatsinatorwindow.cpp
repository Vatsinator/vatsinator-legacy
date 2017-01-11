/*
 * vatsinatorwindow.cpp
 * Copyright (C) 2012 Michał Garapich <michal@garapich.pl>
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

#include "vatsinatorwindow.h"
#include "ui_vatsinatorwindow.h"
#include "aboutwindow.h"
#include "settingswindow.h"
#include <core/option.h>
#include <core/pluginfinder.h>
#include <core/resourcefile.h>
#include <core/servertracker.h>
#include <gui/mapaddon.h>
#include <gui/mapdrawer.h>
#include <gui/mapscene.h>
#include <gui/maprenderer.h>
#include <widgets/airportdetailswindow.h>
#include <widgets/atcdetailswindow.h>
#include <widgets/firdetailswindow.h>
#include <widgets/flightdetailswindow.h>
#include <QtWidgets>

using namespace Vatsinator::Core;
using namespace Vatsinator::Gui;
using namespace Vatsinator::Widgets;

/**
 * The default MapDrawer plugin.
 */
constexpr auto MapDrawerDefaultPlugin = "TiledMapDrawer";


VatsinatorWindow::VatsinatorWindow() :
    QMainWindow(nullptr),
    ui(new Ui::VatsinatorWindow),
    m_server(new ServerTracker(this))
{
    ui->setupUi(this);
    
    connect(ui->map, &MapWidget::airportDetailsRequested, this, &VatsinatorWindow::showAirportDetails);
    connect(ui->map, &MapWidget::flightDetailsRequested, this, &VatsinatorWindow::showFlightDetails);
    connect(ui->map, &MapWidget::atcDetailsRequested, this, &VatsinatorWindow::showAtcDetails);
    connect(ui->map, &MapWidget::firDetailsRequested, this, &VatsinatorWindow::showFirDetails);
    connect(ui->map, &MapWidget::metarRequested, this, qOverload<const QString&>(&VatsinatorWindow::showMetarWindow));
    connect(ui->actionSettings, &QAction::triggered, this, &VatsinatorWindow::showSettingsWindow);
    connect(ui->actionRefresh, &QAction::triggered, m_server, &ServerTracker::refreshData);
    connect(ui->actionMetars, &QAction::triggered, this, qOverload<>(&VatsinatorWindow::showMetarWindow));
    connect(ui->actionClients, &QAction::triggered, this, &VatsinatorWindow::showClientListWindow);
    connect(ui->actionQuit, &QAction::triggered, qApp, &QCoreApplication::quit);
    
    AboutWindow* about = new AboutWindow;
    connect(qApp, &QCoreApplication::aboutToQuit, about, &QObject::deleteLater);
    connect(ui->actionAbout, &QAction::triggered, about, &QWidget::show);
    
    ui->map->setRenderer(new MapRenderer(this));
    ui->map->renderer()->setScene(new MapScene(this));
    
    ui->map->renderer()->scene()->track(m_server);
    m_server->trackServer(QUrl("https://status.vatsim.net/status.txt"));
    
    ModelMatcher* modelMatcher = new ModelMatcher;
    ResourceFile* modelFile = new ResourceFile("data/model.json");
    modelMatcher->setResourceFile(modelFile);
    ui->map->renderer()->scene()->setModelMatcher(modelMatcher);
    
    m_mapInfo = new MapInfoWidget;
    connect(m_server, &ServerTracker::dataFileDownloadFinished, this, &VatsinatorWindow::updateMapInfo);
    
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addItem(new QSpacerItem(0, 20));
    layout->addWidget(m_mapInfo);
    layout->setStretch(0, 1);
    layout->setSpacing(0);
    layout->setContentsMargins(QMargins(0, 0, 0, 0));
    ui->map->setLayout(layout);
}

VatsinatorWindow::~VatsinatorWindow() {}

void VatsinatorWindow::showEvent(QShowEvent* event)
{
    if (ui->map->renderer()->mapDrawer() == nullptr) {
        QSettings s;

        restoreGeometry(s.value("mainwindow/geometry").toByteArray());

        QPointF center = s.value("map/center").toPointF();
        ui->map->renderer()->setCenter(center);
        qreal zoom = s.value("map/zoom", 1.0).toReal();
        ui->map->renderer()->setZoom(zoom);

        Option* mapDrawerPlugin = new Option("plugins/map_drawer", QString(MapDrawerDefaultPlugin), this);
        connect(mapDrawerPlugin, &Option::valueChanged, this, &VatsinatorWindow::setMapDrawerPlugin);
        setMapDrawerPlugin(mapDrawerPlugin->value());

        auto addons = PluginFinder::pluginsForIid(qobject_interface_iid<MapAddon*>());
        for (auto a: addons) {
            ui->map->renderer()->attachMapAddon(qobject_cast<MapAddon*>(a));
        }
    }

    event->accept();
}

void VatsinatorWindow::closeEvent(QCloseEvent* event)
{
    QSettings s;
    s.setValue("map/center", ui->map->renderer()->center());
    s.setValue("map/zoom", ui->map->renderer()->zoom());
    
    s.setValue("mainwindow/geometry", saveGeometry());
    
    event->accept();
}

void VatsinatorWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateMapInfo();
        event->accept();
    } else {
        event->ignore();
    }
}

void VatsinatorWindow::showAirportDetails(const AirportObject* airport)
{
    AirportDetailsWindow* w = new AirportDetailsWindow();
    connect(w, &AirportDetailsWindow::clientDetailsRequested, this, &VatsinatorWindow::showClientDetails);
    connect(w, &AirportDetailsWindow::showOnMapRequested, [w, this]() {
        w->close();
        LonLat p = w->airport()->position();
        this->ui->map->renderer()->moveTo(p);
    });
    w->setAirport(airport);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
}

void VatsinatorWindow::showFlightDetails(const Pilot* pilot)
{
    FlightDetailsWindow* w = new FlightDetailsWindow();
    connect(w, &FlightDetailsWindow::airportDetailsRequested, this, &VatsinatorWindow::showAirportDetails);
    connect(w, &FlightDetailsWindow::showOnMapRequested, [w, this]() {
        w->close();
        LonLat p = w->flight()->position();
        this->ui->map->renderer()->moveTo(p);
    });
    connect(w, &FlightDetailsWindow::flightTrackingToggled, [w, this](bool tracked) {
        this->ui->map->renderer()->scene()->setTrackedFlight(tracked ? w->flight() : nullptr);
    });
    w->setFlight(pilot);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
}

void VatsinatorWindow::showAtcDetails(const Atc* atc)
{
    AtcDetailsWindow* w = new AtcDetailsWindow();
    w->setAtc(atc);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
}

void VatsinatorWindow::showFirDetails(const FirObject* fir)
{
    FirDetailsWindow* w = new FirDetailsWindow();
    connect(w, &FirDetailsWindow::clientDetailsRequested, this, &VatsinatorWindow::showClientDetails);
    w->setFir(fir);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
}

void VatsinatorWindow::showClientDetails(const QString& callsign)
{
    const Client* client = m_server->findClient(callsign);
    Q_ASSERT(client);
    
    if (const Pilot* pilot = qobject_cast<const Pilot*>(client))
        showFlightDetails(pilot);
    else if (const Atc* atc = qobject_cast<const Atc*>(client))
        showAtcDetails(atc);
    else
        Q_UNREACHABLE();
}

void VatsinatorWindow::updateMapInfo()
{
    m_mapInfo->setClients(m_server->clientCount());
    m_mapInfo->setPilots(m_server->pilotCount());
    m_mapInfo->setAtcs(m_server->atcCount());
    m_mapInfo->setObservers(m_server->observerCount());
    m_mapInfo->setUpdated(m_server->lastUpdate());
}

void VatsinatorWindow::showSettingsWindow()
{
    if (!m_settings) {
        m_settings = new SettingsWindow;
        connect(qApp, &QCoreApplication::aboutToQuit, m_settings, &QObject::deleteLater);
    }
    
    Q_ASSERT(m_settings);
    m_settings->show();
}

void VatsinatorWindow::setMapDrawerPlugin(const QVariant& name)
{
    MapDrawer* drawer = qobject_cast<MapDrawer*>(PluginFinder::pluginByName(name.toString()));
    ui->map->renderer()->setMapDrawer(drawer);
}

void VatsinatorWindow::showMetarWindow()
{
    if (!m_metars) {
        m_metars = new MetarWindow(m_server->metarManager());
        connect(qApp, &QCoreApplication::aboutToQuit, m_metars, &QObject::deleteLater);
    }

    Q_ASSERT(m_metars);
    m_metars->show();
}

void VatsinatorWindow::showMetarWindow(const QString& icaoSearch)
{
    if (!m_metars) {
        m_metars = new MetarWindow(m_server->metarManager());
        connect(qApp, &QCoreApplication::aboutToQuit, m_metars, &QObject::deleteLater);
    }

    Q_ASSERT(m_metars);
    m_metars->show(icaoSearch);
}

void VatsinatorWindow::showClientListWindow()
{
    if (!m_clients) {
        m_clients = new ClientListWindow(m_server);
        connect(m_clients, &ClientListWindow::clientDetailsRequested, this, &VatsinatorWindow::showClientDetails);
        connect(qApp, &QCoreApplication::aboutToQuit, m_clients, &QObject::deleteLater);
    }

    Q_ASSERT(m_clients);
    m_clients->show();
}
