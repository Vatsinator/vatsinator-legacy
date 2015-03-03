/*
    main.cpp
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtCore>
#include <QtPlugin>
#include <QVariantAnimation>
#include "network/resourcemanager.h"
#include "vatsimdata/lonlat.h"
#include "vatsinatorapplication.h"
#include "config.h"


Q_IMPORT_PLUGIN(UltimateWeatherForecasts)

/**
 * The interpolator for LonLat, so that it can be animated nicely, i.e. by MapScene.
 */
QVariant lonLatInterpolator(const LonLat& start, const LonLat& end, qreal progress) {
  return LonLat(
    start.longitude() + (end.longitude() - start.longitude()) * progress,
    start.latitude() + (end.latitude() - start.latitude()) * progress
  );
}

int main(int argc, char** argv) {
  Q_INIT_RESOURCE(files);
  Q_INIT_RESOURCE(imgs);
  Q_INIT_RESOURCE(shaders);
  Q_INIT_RESOURCE(weather);
  
  QCoreApplication::setApplicationName(QStringLiteral("Vatsinator"));
  QCoreApplication::setOrganizationName(QStringLiteral("VatsinatorTeam"));
  QCoreApplication::setApplicationVersion(VATSINATOR_VERSION);
  QCoreApplication::setOrganizationDomain(QStringLiteral("org.eu.vatsinator"));
  
  qRegisterMetaType<LonLat>("LonLat");
  qRegisterMetaTypeStreamOperators<LonLat>("LonLat");
  qRegisterAnimationInterpolator<LonLat>(lonLatInterpolator);
  
  qRegisterMetaType<ResourceManager::VersionStatus>("ResourceManager::VersionStatus");
  
  /* Defined in events/types.cpp */
  extern void init_event_types();
  init_event_types();
  
  return VatsinatorApplication(argc, argv).exec();
}
