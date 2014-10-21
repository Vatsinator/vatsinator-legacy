/*
 * vatsinatorwindow.h
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

#ifndef VATSINATORWINDOW_H
#define VATSINATORWINDOW_H

#include <QMainWindow>

#include "singleton.h"
#include "ui/ui_vatsinatorwindow.h"

class QLabel;
class QProgressBar;
class MouseLonLatEvent;

class VatsinatorWindow :
    public QMainWindow,
    private Ui::VatsinatorWindow {
  
  Q_OBJECT

public:
  explicit VatsinatorWindow(QWidget* parent = nullptr);
  
  /**
   * Sets the specified message in the bottom-left bar corner
   * or puts simple "Last update" text.
   * \param text If specified, this text will be shown.
   */
  void statusBarUpdate(const QString& text = "", const QPalette& palette = QPalette());
  
  /**
   * The middle part of status bar - how many clients, etc etc.
   */
  void infoBarUpdate();
  
  /**
   * Custom event handler.
   */
  bool event(QEvent* event) override;
  
  inline MapWidget* mapWidget() { return MapDisplay; }
  inline QProgressBar* progressBar() { return __progressBar; }
  
protected:
  void closeEvent(QCloseEvent* event) override;
  void showEvent(QShowEvent* event) override;
  virtual bool mouseLonLatMoveEvent(MouseLonLatEvent* event);
  
private:
  void __storeWindowGeometry();
  void __restoreWindowGeometry();
  
private slots:
  void __dataDownloading();
  void __statusUpdated();
  void __dataUpdated();
  void __dataCorrupted();
  void __enableRefreshAction();
  
private:  
  QLabel*       __statusBox;
  QProgressBar* __progressBar;
  
  
};

#endif // VATSINATORWINDOW_H
