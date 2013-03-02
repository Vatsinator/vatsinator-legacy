/*
    settingsmanager.h
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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


#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QColor>

#include "singleton.h"

class SettingsWindow;

class SettingsManager : public QObject, public Singleton< SettingsManager > {

  Q_OBJECT

  enum {
    WHEN_HOVERED = 1,
    AIRPORT_RELATED = 2,
    ALWAYS = 4
  };

public:
  struct DisplayLayersPolicy {
    bool  pilots;
    bool  airports;
    bool  staffedFirs;
    bool  unstaffedFirs;
    bool  emptyAirports;
  };

  SettingsManager(QObject* = 0);
  
  void init();

  inline int
  getRefreshRate() const { return __refreshRate; }

  inline bool
  refreshMetars() const { return __metarsRefresh; }

  inline bool
  hasAntyaliasing() const { return __antyaliasing; }
  
  inline bool
  cacheEnabled() const { return __cacheEnabled; }
  
  inline int
  getZoomCoefficient() const { return __zoomCoefficient; }

  inline const DisplayLayersPolicy &
  getDisplayLayersPolicy() const { return __displayLayers; }

  inline bool
  displayAirportLabels() const { return __displayAirportLabels; }

  inline bool
  displayPilotsLabelsWhenHovered() const { return __pilotsLabelsDisplayPolicy & WHEN_HOVERED; }

  inline bool
  displayPilotsLabelsAirportRelated() const { return __pilotsLabelsDisplayPolicy & AIRPORT_RELATED; }

  inline bool
  displayPilotsLabelsAlways() const { return __pilotsLabelsDisplayPolicy & ALWAYS; }

  inline const QColor&
  getUnstaffedFirBordersColor() const { return __unstaffedFirBordersColor; }

  inline const QColor&
  getStaffedFirBordersColor() const { return __staffedFirBordersColor; }

  inline const QColor&
  getStaffedFirBackgroundColor() const { return __staffedFirBackgroundColor; }

  inline const QColor&
  getStaffedUirBordersColor() const { return __staffedUirBordersColor; }

  inline const QColor&
  getStaffedUirBackgroundColor() const { return __staffedUirBackgroundColor; }

  inline const QColor&
  getApproachCircleColor() const { return __approachCircleColor; }

  inline const QColor&
  getSeasColor() const { return __seasColor; }

  inline const QColor&
  getLandsColor() const { return __landsColor; }

  inline const QColor&
  getOriginToPilotLineColor() const { return __originToPilotLineColor; }

  inline const QColor&
  getPilotToDestinationLineColor() const { return __pilotToDestinationLineColor; }
  
  inline const QString&
  getLanguage() const { return __language; }

signals:
  void settingsRestored();
  void settingsChanged();

private:
  /* Saves all settings in the system, using QSettings */
  void __saveSettings();

  /* Restores settings from the registry/config files */
  void __restoreSettings();

  /* Clears all config entries from the registry/config files */
  void __clearEntries();

  SettingsWindow*   __mySettingsWindow;

  /* Miscallaneous */
  int                 __refreshRate;
  bool                __metarsRefresh;
  bool                __antyaliasing;
  bool                __cacheEnabled;
  int                 __zoomCoefficient;
  DisplayLayersPolicy __displayLayers;
  unsigned            __pilotsLabelsDisplayPolicy;
  bool                __displayAirportLabels;

  /* Various map colors */
  QColor __unstaffedFirBordersColor;
  QColor __staffedFirBordersColor;
  int    __staffedFirBackgroundAlpha;
  QColor __staffedFirBackgroundColor;
  QColor __staffedUirBordersColor;
  int    __staffedUirBackgroundAlpha;
  QColor __staffedUirBackgroundColor;
  QColor __approachCircleColor;
  QColor __seasColor;
  QColor __landsColor;
  QColor __originToPilotLineColor;
  QColor __pilotToDestinationLineColor;
  
  QString __language;

private slots:
  void __updateSettings();
  void __restoreDefaults();


};

#endif // SETTINGSMANAGER_H
