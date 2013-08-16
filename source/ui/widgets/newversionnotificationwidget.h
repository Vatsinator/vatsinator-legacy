/*
    newversionnotificationwidget.h
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


#ifndef NEWVERSIONNOTIFICATIONWIDGET_H
#define NEWVERSIONNOTIFICATIONWIDGET_H

#include "ui/widgets/notificationwidget.h"
#include "ui/ui_newversionnotificationwidget.h"

class NewVersionNotificationWidget :
    public NotificationWidget,
    private Ui::NewVersionNotificationWidget {
  
  /*
   * This is the red widget that opens up if UpdateChecker detects
   * outdated version of running Vatsinator.
   */
  
  Q_OBJECT
  
public:
  explicit NewVersionNotificationWidget();
  
protected:
  void hideEvent(QHideEvent*);
  
};

#endif // NEWVERSIONNOTIFICATIONWIDGET_H
