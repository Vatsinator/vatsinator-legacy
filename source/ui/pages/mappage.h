/*
 * mappage.h
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#ifndef MAPPAGE_H
#define MAPPAGE_H

#include <QFont>
#include <QWidget>

#include "ui/ui_mappage.h"
#include "storage/abstractsettingsmodule.h"

class MapPage : public QWidget, public AbstractSettingsModule, private Ui::MapPage {
  
  Q_OBJECT

public:
  MapPage(QWidget* = nullptr);
  
  QString listElement() const override;
  QString listIcon() const override;
  QString moduleId() const override;
  
  /**
   * @variables
   * zoom_coefficient:                  int
   * staffed_fir_borders_color:         QColor
   * staffed_fir_background_color:      QColor
   * unstaffed_fir_borders_color:       QColor
   * staffed_uir_borders_color:         QColor
   * staffed_uir_background_color:      QColor
   * fir_font:                          QFont
   * airport_font:                      QFont
   * pilot_font:                        QFont
   * approach_circle_color:             QColor
   * seas_color:                        QColor
   * lands_color:                       QColor
   * origin_to_pilot_line_color:        QColor
   * pilot_to_destination_line_color:   QColor
   */
  void update() const override;
  
protected:
  void restore(QSettings&) override;
  void save(QSettings&) override;
  
private:
  void __updateFontButtons();
  
private slots:
  void __showFirFontDialog();
  void __showAirportFontDialog();
  void __showPilotFontDialog();
  
private:
  QFont __firFont;
  QFont __airportFont;
  QFont __pilotFont;
  
};

#endif // MAPPAGE_H
