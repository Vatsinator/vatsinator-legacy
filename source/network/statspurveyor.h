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

class QNetworkReply;

/**
 * The StatsPurveyor class is used to send anonymous statistics to Vatsinator servers.
 * If user does not want to send any stats, requests will be quietly dropped.
 * If user did not make any decision yet, requests will be queued and sent
 * when he accepts them. Otherwise requests will be sent as soon as possible.
 * All functions are thread-safe.
 */
class StatsPurveyor : public QObject {
  Q_OBJECT
  Q_ENUMS(UserDecision)
  
  /**
   * Holds the user setting.
   */
  Q_PROPERTY(UserDecision userDecision READ userDecision WRITE setUserDecision)

signals:
  /**
   * Emitted when new reuqest is enqueued.
   */
  void newRequest();

public:
  /**
   * The UserDecision enum describes user choice.
   */
  enum UserDecision {
    Accepted,   /**< User accepted statistics */
    Declined,   /**< User declined sending anything */
    NotYetMade  /**< User did not make the decision yet */
  };
  
  /**
   * The default constructor passes _parent_ to QObject.
   */
  explicit StatsPurveyor(QObject* parent = nullptr);
  
  virtual ~StatsPurveyor();
  
  void setUserDecision(UserDecision decision);
  inline UserDecision userDecision() const { return __userDecision; }
  
public slots:
  /**
   * Reports application startup; invoked automatically.
   */
  void reportStartup();
  
  /**
   * Use this function to report unrecognized ATC callsigns.
   */
  void reportNoAtc(const QString& callsign);

private:
  
  void __enqueueRequest(const QNetworkRequest& request);
  
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
   * Starts new request if nothing is being sent at the moment.
   */
  void __nextIfFree();
  
private:
  UserDecision __userDecision;
  
  /* networkAccessible = NotAccessible when user disables stats */
  QNetworkAccessManager __nam;
  
  /* Requests to be sent */
  QQueue<QNetworkRequest> __requests;
  
  QNetworkReply* __reply;

};

#endif // STATSPURVEYOR_H
