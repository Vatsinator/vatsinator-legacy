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

#include "ui/ui_vatsinatorwindow.h"

class MapInfoWidget;
class MouseLonLatEvent;

/**
 * The main application window.
 */
class VatsinatorWindow : public QMainWindow, private Ui::VatsinatorWindow {
    Q_OBJECT
    
public:
    explicit VatsinatorWindow(QWidget* parent = nullptr);
    
    inline MapWidget* mapWidget()
    {
        return MapDisplay;
    }
    
protected:
    void customEvent(QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
    virtual void mouseLonLatMoveEvent(MouseLonLatEvent* event);
    
private:
    void __storeWindowGeometry();
    void __restoreWindowGeometry();
    
private:
    MapInfoWidget* __mapInfo;
    
};

#endif // VATSINATORWINDOW_H
