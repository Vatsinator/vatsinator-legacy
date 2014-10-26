/*
 * abstractnotamprovider.h
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

#ifndef ABSTRACTNOTAMPROVIDER_H
#define ABSTRACTNOTAMPROVIDER_H

#include <QObject>

#include "vatsimdata/models/notamlistmodel.h"

/**
 * This class is the interface for all NOTAMs providers.
 * 
 * Each NOTAM provider is responsible for creating new models when
 * necessary, filling them in and giving access to them. User requests
 * NOTAMs calling fetchNotam() method. Pointers are given by emiting
 * notamReady() signal.
 * 
 * \todo Rename to NotamProviderInterface and make a plugin here.
 */
class AbstractNotamProvider : public QObject {
  Q_OBJECT
  
signals:
  /**
   * Signal emited when NOTAMs requested by fetchNotam() method are ready
   * to read.
   * 
   * \sa fetchNotam().
   */
  void notamReady(NotamListModel* model);
  
public:
  /**
   * The default constructor passes _parent_ to QObject.
   */
  explicit AbstractNotamProvider(QObject* parent = nullptr);
  
  /**
   * Requests NOTAM for the given ICAO code. The ICAO can be either airport's
   * or FIR's. When the NOTAM report is ready, notamReady() signal must
   * be emitted.
   * 
   * \param icao ICAO code.
   * \sa notamReady().
   */
  virtual void fetchNotam(const QString& icao) = 0;
  
  /**
   * Description text for the provider.
   * This is rich-formatted text that can contain URL, image or anything
   * Qt can recognize in a QLabel.
   * 
   * Default implementation returns an empty string.
   */
  virtual QString providerInfo() const;
  
};

#endif // ABSTRACTNOTAMPROVIDER_H
