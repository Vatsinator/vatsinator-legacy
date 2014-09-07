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
 */
class Airline : public QObject {
  Q_OBJECT
  
signals:
  
  /**
   * Emitted when logo is downloaded and loaded (if exists).
   */
  void logoFetched();
  
public:
  
  /**
   * @param icao Airline ICAO code.
   * @param name Airline name.
   * @param country Country where the airline comes from.
   * @param website The current airline website.
   * @param logo The logo url.
   * @param parent QObject's parent.
   */
  Airline(QString, QString, QString, QString, QString, QObject* = nullptr);
  
  /**
   * Constructs the airline from the JSON object.
   * @param json The JSON object that contains all airline data
   * @param parent QObject's parent.
   */
  Airline(const QJsonObject&, QObject* = nullptr);
  
  /**
   * Airline's logo is lazy-loaded. To have it fetched from the internet
   * or loaded from cache, call this function.
   * @sa logoFetched().
   */
  void requestLogo();
  
  /**
   * Gets the airline ICAO code.
   */
  inline const QString& icao() const { return __icao; }
  
  /**
   * Gets the airline name.
   */
  inline const QString& name() const { return __name; }
  
  /**
   * Gets the country that the airline comes from.
   */
  inline const QString& country() const { return __country; }
  
  /**
   * Gets the airline website url.
   */
  inline const QString& website() const { return __website; }
  
  /**
   * Gets the airline logo image.
   * @Note: This image is valid only after the logoFetched() signal is emitted.
   */
  inline const QImage& logo() const { return __logo; }
  
private slots:
  void __logoFetched(QString);
  
private:
  QString __icao;
  QString __name;
  QString __country;
  QString __website;
  QString __logoUrl;
  QImage  __logo;

};

#endif // AIRLINE_H
