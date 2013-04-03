/*
    miscellaneouspage.h
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

#ifndef MISCELLANEOUSPAGE_H
#define MISCELLANEOUSPAGE_H

#include "ui/ui_miscellaneouspage.h"
#include "ui/pages/abstractsettingspage.h"

class MiscellaneousPage :
    public AbstractSettingsPage,
    private Ui::MiscellaneousPage {
  
  Q_OBJECT
  VATSINATOR_DECLARE_PAGE(Misc)
  
public:
  
  MiscellaneousPage(QWidget* = 0);
  
  QString listElement() const;
  QString listIcon() const;
  
  /**
   * @variables
   * refresh_rate:      int
   * refresh_metars:    bool
   * has_antyaliasing:  bool
   * cache_enabled:     bool
   * zoom_coefficient:  int
   * language:          QString
   */
  QVariant get(const QString&) const;

protected:
  void __restore(QSettings&);
  void __save(QSettings&);
  
};

#endif // MISCELLANEOUSPAGE_H
