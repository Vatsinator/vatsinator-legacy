/*
 * vatsimstatusdocument.h
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

#ifndef VATSIMSTATUSDOCUMENT_H
#define VATSIMSTATUSDOCUMENT_H

#include <QObject>
#include <QList>
#include <QUrl>

/**
 * The VatsimStatusDocument class represents a status.txt file that is
 * downloaded from VATSIM servers at the program startup.
 */
class VatsimStatusDocument : public QObject {
    Q_OBJECT
    
    /**
     * This property specifies whether the status file is correct or not.
     */
    Q_PROPERTY(bool valid READ isValid)

public:
  /**
   * Creates the new document from the given _data_, which is file contents.
   */
  explicit VatsimStatusDocument(QByteArray data, QObject* parent = nullptr);
  
  /**
   * URLs where complete data files are available (_url0_ directive).
   */
  inline const QList<QUrl>& dataFileUrls() const { return __dataFileUrls; }
  
  /**
   * URLs where servers list data files are available (_url1_ directive).
   */
  inline const QList<QUrl>& serverListUrls() const { return __serverListUrls; }
  
  /**
   * Returns data which was passed to the constructor.
   */
  inline const QByteArray& data() const { return __data; }
  
  inline bool isValid() const { return __isValid; }
  
  /**
   * URL where to retrieve metar.
   */
  inline const QUrl& metar() const { return __metar; }
  
  /**
   * Message to be displayed at application startup.
   */
  inline const QString& message() const { return __message; }
  
  /**
   * URL where to retrieve a more updated status.txt file that overrides this
   * one (_moveto0_ directive).
   */
  inline const QUrl& moveTo() const { return __moveTo; }
  
private:
  void __parse();
  
  QByteArray __data;
  bool __isValid;
  
  QList<QUrl> __dataFileUrls;
  QList<QUrl> __serverListUrls;
  QUrl __metar;
  QString __message;
  QUrl __moveTo;
  
};

#endif // VATSIMSTATUSDOCUMENT_H
