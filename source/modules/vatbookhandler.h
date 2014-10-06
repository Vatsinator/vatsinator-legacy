/*
    vatbookhandler.h
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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


#ifndef VATBOOKHANDLER_H
#define VATBOOKHANDLER_H

#include <QMap>
#include <QTimer>
#include <QObject>
#include <QString>

#include "singleton.h"

class BookedAtcTableModel;
class PlainTextDownloader;

/**
 * The VatbookHandler class integrates Vatsinator with the VATBOOK service.
 * \todo Make booking plugin interface.
 * \todo Remove Singleton inheritance.
 */
class VatbookHandler : public QObject, public Singleton<VatbookHandler> {  
  Q_OBJECT
  
public:
  
  /**
   * The default constructor passes _parent_ to QObject.
   */
  explicit VatbookHandler(QObject* parent = nullptr);
  
  /**
   * The destructor.
   */
  virtual ~VatbookHandler();
  
  /**
   * Finds the model by given airport or FIR ICAO code.
   * 
   * \param icao The lookup ICAO code.
   * \sa notNullModel().
   */
  BookedAtcTableModel* model(const QString& icao);
  
  /**
   * If the model does not exist, this method returns just empty model
   * (without any data), which is better for views, as _nullptr_ makes
   * the headers disappear.
   * 
   * \param icao The lookup ICAO code.
   */
  BookedAtcTableModel* notNullModel(const QString& icao);
  
private:
  void __clear();
  void __parseData(const QString& data);
  
private slots:
  void __dataFetched(const QString& data);
  void __handleError();
  void __timeToUpdate();

private:  
  /* This map contains pairs airport/fir icao <-> atcs */
  QMap<QString, BookedAtcTableModel*> __bookings;
  
  PlainTextDownloader* __downloader;
  QTimer               __timer;
  
};

#endif // VATBOOKHANDLER_H
