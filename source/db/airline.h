/*
 * airline.h
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

#ifndef AIRLINE_H
#define AIRLINE_H

#include <QObject>
#include <QImage>

/**
 * The Airline class represents a single airline.
 * To get the Airline instance, you can use AirlineDatabase::find() method.
 * Airlines can have logos that are provided by the VatsinatorDatabase service.
 * By default, the Airline instance won't have the logo fetched. To request
 * the logo, call requestLogo() method. It will be then loaded from cache or
 * downloaded from the internet. requestLogo() is asynchronous method; when
 * the logo is loaded, logoFetched() signal will be emitted.
 * 
 * \sa AirlineDatabase and VatsimDataHandler.
 */
class Airline : public QObject {
  Q_OBJECT
  
  /**
   * Stores the airline ICAO code (3 letters).
   */
  Q_PROPERTY(QString icao READ icao)
  
  /**
   * Stores the full airline name.
   */
  Q_PROPERTY(QString name READ name)
  
  /**
   * Keeps the country that the airline comes from.
   */
  Q_PROPERTY(QString country READ country)
  
  /**
   * Holds the URL of the airline's webpage.
   */
  Q_PROPERTY(QString website READ website)
  
  /**
   * Holds the airline's logo.
   * By default, the logo is empty (that is, logo().isNull() returns true).
   * 
   * \sa requestLogo().
   */
  Q_PROPERTY(QImage logo READ logo NOTIFY logoFetched)
  
signals:
  
  /**
   * Emitted when logo is downloaded and loaded (if exists).
   * 
   * \sa requestLogo().
   */
  void logoFetched();
  
public:
  
  /**
   * \param icao Airline ICAO code.
   * \param name Airline name.
   * \param country Country where the airline comes from.
   * \param website The current airline website.
   * \param logo The logo url.
   * \param parent QObject's parent.
   */
  Airline(QString icao, QString name, QString country, QString website,
          QString logo, QObject* parent = nullptr);
  
  /**
   * Constructs the airline from the JSON object.
   * \param json The JSON object that contains all airline data.
   * \param parent QObject's parent.
   */
  Airline(const QJsonObject& icao, QObject* parent = nullptr);
  
  /**
   * Airline's logo is lazy-loaded. To have it fetched from the internet
   * or loaded from cache, call this function.
   * 
   * \sa logoFetched().
   */
  void requestLogo();
  
  inline const QString& icao() const { return __icao; }
  inline const QString& name() const { return __name; }
  inline const QString& country() const { return __country; }
  inline const QString& website() const { return __website; }
  inline const QImage& logo() const { return __logo; }
  
private slots:
  void __logoFetched(QString fileName);
  
private:
  QString __icao;
  QString __name;
  QString __country;
  QString __website;
  QString __logoUrl;
  QImage  __logo;

};

#endif // AIRLINE_H
