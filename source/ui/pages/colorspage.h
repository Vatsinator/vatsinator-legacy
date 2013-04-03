/*
    colorspage.h
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

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


#ifndef COLORSPAGE_H
#define COLORSPAGE_H

#include "ui/ui_colorspage.h"
#include "ui/pages/abstractsettingspage.h"

class ColorsPage :
    public AbstractSettingsPage,
    private Ui::ColorsPage {
  
  Q_OBJECT
  VATSINATOR_DECLARE_PAGE(Colors)

public:
  
  ColorsPage(QWidget* = 0);
  
  QString listElement() const;
  QString listIcon() const;
  
  /**
   * @variables
   * staffed_fir_borders:       QColor
   * staffed_fir_background:    QColor
   * unstaffed_fir_borders:     QColor
   * staffed_uir_borders:       QColor
   * staffed_uir_background;    QColor
   * approach_circle:           QColor
   * seas:                      QColor
   * lands:                     QColor
   * origin_to_pilot_line:      QColor
   * pilot_to_destination_line: QColor
   */
  QVariant get(const QString&) const;
  
protected:
  void __restore(QSettings&);
  void __save(QSettings&);
  
};

#endif // COLORSPAGE_H
