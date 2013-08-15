/*
    dataupdatenotificationwidget.h
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


#ifndef DATAUPDATENOTIFICATIONWIDGET_H
#define DATAUPDATENOTIFICATIONWIDGET_H

#include "ui/widgets/abstractnotificationwidget.h"
#include "ui/ui_dataupdatenotificationwidget.h"

class DataUpdater;

class DataUpdateNotificationWidget :
    public AbstractNotificationWidget,
    private Ui::DataUpdateNotificationWidget {
  
  /*
   * This notification shows data download progress, extracting
   * progress and some more additional info.
   */
  
  Q_OBJECT
  
public:
  
  explicit DataUpdateNotificationWidget(DataUpdater*);
  
  AbstractNotificationWidget::Position position() const;
  
  inline QProgressBar *
    downloadBar() {
      return DownloadBar;
    }
  
  inline const QProgressBar *
    downloadBar() const {
      return DownloadBar;
    }
  
public slots:
  
  void downloadPage();
  
};

#endif // DATAUPDATENOTIFICATIONWIDGET_H
