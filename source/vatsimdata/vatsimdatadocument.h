/*
 * vatsimdatadocument.h
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

#ifndef VATSIMDATADOCUMENT_H
#define VATSIMDATADOCUMENT_H

#include <QObject>

/**
 * The VatsimDataDocument class represents a single data file that is
 * being fetched periodically from the internet. It contains list of connected
 * clients and prefiles.
 */
class VatsimDataDocument : public QObject {
    Q_OBJECT
    
    /**
     * Indicates whether the document is valid and parsed successfully.
     */
    Q_PROPERTY(bool valid READ isValid)

public:
  /**
   * Struct that keeps some raw info about client that we need during data file
   * parsing process.
   */
  struct ClientLine {
    ClientLine(const QString& data);
    
    QStringList line; /**< Split line */
    bool valid; /**< Indicates whether the line is correct or not  */
    QString callsign; /**< Parsed client callsign */
    enum { Pilot, Atc } type; /**< Parsed client type */
  };
  
  VatsimDataDocument(QByteArray data, QObject* parent = nullptr);
  
  inline bool isValid() const { return __isValid; }
  
  /**
   * Time in minutes the data file will be updated.
   */
  inline int reload() const { return __reload; }
  
  /**
   * The last date and time this document has been updated.
   */
  inline const QDateTime& update() const { return __update; }
  
  /**
   * The number of clients currently connected.
   */
  inline int connectedClients() const { return __connectedClients; }
  
  /**
   * Gives direct access to list of raw client data.
   * Each client is guaranteed to be valid.
   */
  inline const QList<ClientLine>& clients() const { return __clients; }
  
  /**
   * Gives direct access to list of prefiled flights.
   * Each prefile is guaranteed to be valid.
   */
  inline const QList<ClientLine>& prefile() const { return __prefile; }
  
private:
  void __parse();
  
  QByteArray __data;
  bool __isValid;
  
  int __version;
  int __reload;
  QDateTime __update;
  int __connectedClients;
  QList<ClientLine> __clients;
  QList<ClientLine> __prefile;
    
};

#endif // VATSIMDATADOCUMENT_H
