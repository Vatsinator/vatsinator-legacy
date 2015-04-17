/*
 * notamprovider.h
 * Copyright (C) 2014-2015  Michał Garapich <michal@garapich.pl>
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

#ifndef NOTAMPROVIDER_H
#define NOTAMPROVIDER_H

#include <QObject>
#include <QtPlugin>

#include "plugins/notamlistmodel.h"

/**
 * This class is the interface for all NOTAM providers.
 * 
 * Each NOTAM provider is responsible for creating new models when
 * necessary, filling them in and giving access to them. User requests
 * NOTAMs calling fetchNotam() method. Pointers are given by emiting
 * notamReady() signal.
 */
class NotamProvider : public QObject {
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
  using QObject::QObject;
  
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
   */
  virtual QString providerInfo() const = 0;
  
};

Q_DECLARE_INTERFACE(NotamProvider,
                    "org.eu.vatsinator.Vatsinator.NotamProvider")

#endif // NOTAMPROVIDER_H
