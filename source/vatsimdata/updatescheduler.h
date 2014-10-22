/*
 * updatescheduler.h
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

#ifndef UPDATESCHEDULER_H
#define UPDATESCHEDULER_H

#include <QObject>
#include <QTimer>

/**
 * UpdateScheduler decides when to update Vatsim data.
 * Instance of this class is used by VatsimDataHandler.
 * 
 * timeToUpdate() signal is emitted when the UpdateScheduler decides
 * that it is time to update the data. The timer is then stopped and
 * restarted when VatsimDataHandler emits vatsimDataUpdated() signal.
 */
class UpdateScheduler : public QObject {
  Q_OBJECT
  
signals:
  void timeToUpdate();
  
public:
  explicit UpdateScheduler(QObject* parent = 0);
  
private slots:
  void __setupTimer();
  
private:
  QTimer __timer;
  
};

#endif // UPDATESCHEDULER_H
