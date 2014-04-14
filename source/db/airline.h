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
   * @param icao
   * @param name
   * @param country
   * @param website
   * @param logo
   */
  Airline(QString, QString, QString, QString, QString, QObject* = nullptr);
   
  inline const QString& icao() const { return __icao; }
  inline const QString& name() const { return __name; }
  inline const QString& country() const { return __country; }
  inline const QString& website() const { return __website; }
  inline const QImage& logo() const { return __logo; }
  
public slots:
  void fetchLogo();
  
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
