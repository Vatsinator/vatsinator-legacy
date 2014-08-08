/*
    viewpage.h
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


#ifndef VIEWPAGE_H
#define VIEWPAGE_H

#include <QWidget>

#include "ui/ui_viewpage.h"
#include "storage/abstractsettingsmodule.h"

class ViewPage : public QWidget, public AbstractSettingsModule, private Ui::ViewPage {
  
  Q_OBJECT
  
public:

  ViewPage(QWidget* = 0);
  
  QString listElement() const;
  QString listIcon() const;
  QString moduleId() const;
  
  /**
   * @variables
   * pilots_layer:                      bool
   * airports_layer:                    bool
   * staffed_firs:                      bool
   * unstaffed_firs:                    bool
   * empty_airports:                    bool
   * 
   * pilot_labels.always:               bool
   * pilot_labels.when_hovered:         bool
   * pilot_labels.airport_related:      bool
   * 
   * airport_labels:                    bool
   */
  void update() const;

protected:
  void restore(QSettings&);
  void save(QSettings&);
  
private slots:
  void __handleAlwaysCheckBox(int);
  
};

#endif // VIEWPAGE_H
