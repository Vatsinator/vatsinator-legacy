/*
 * statspurveyor.h
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

#ifndef STATSPURVEYOR_H
#define STATSPURVEYOR_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QQueue>
#include "singleton.h"

class QNetworkReply;

/**
 * This class is used to send anonymous statistics to Vatsinator
 * server.
 */
class StatsPurveyor :
    public QObject,
    public Singleton<StatsPurveyor> {
  Q_OBJECT
  
signals:
  
  /**
   * Emited when new request is enqueued.
   */
  void newRequest();

public:
  explicit StatsPurveyor(QObject* = 0);
  virtual ~StatsPurveyor();
  
public slots:
  
  /**
   * Reports application startup.
   */
  void reportStartup();
  
  /**
   * If an ATC has unknown callsign, we should report it.
   */
  void reportNoAtc(const QString&);

private:
  void __enqueueRequest(const QNetworkRequest&);
  
private slots:
  
  /**
   * Usually JSON-formatted response.
   */
  void __parseResponse();
  
  /**
   * Dequeues next request and executes it.
   */
  void __nextRequest();
  
  /**
   * Honors user's settings.
   */
  void __applySettings();
  
  /**
   * Connected to the LetSendStatsDialog.
   */
  void __statsAccepted();
  void __statsRejected();
  
  /**
   * Starts new request if nothing is being sent at the moment.
   */
  void __nextIfFree();
  
private:
  
  /* networkAccessible = NotAccessible when user disables stats */
  QNetworkAccessManager __nam;
  
  /* Requests to be sent */
  QQueue<QNetworkRequest> __requests;
  
  QNetworkReply* __reply;

};

#endif // STATSPURVEYOR_H
