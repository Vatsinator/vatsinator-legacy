/*
 * dataupdater.h
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#ifndef DATAUPDATER_H
#define DATAUPDATER_H

#include <QObject>

class DataUpdateNotificationWidget;

class DataUpdater : public QObject {
  
  /**
   * The DataUpdater class is responsible for downloading the data package,
   * unpacking it, checking the manifest file, copying modified files to
   * local directory and restarting the application, simultaneously
   * showing the appropraite notification.
   */
  
  Q_OBJECT

signals:
  
  void downloading();
  
public:
  
  explicit DataUpdater(QObject* = 0);
  
  virtual ~DataUpdater();
  
private:
  
  DataUpdateNotificationWidget* __notification;
  
private slots:
  
  void __startDownload();
  void __downloadFinished();

};

#endif // DATAUPDATER_H
