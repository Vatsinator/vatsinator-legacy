/*
 * metarupdater.h
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#ifndef METARUPDATER_H
#define METARUPDATER_H

#include <QObject>
#include <QQueue>
#include <QString>

class PlainTextDownloader;
class MetarListModel;

class MetarUpdater : public QObject {
    Q_OBJECT
    
public:
    MetarUpdater(MetarListModel* model, QObject* parent = nullptr);
    
    inline MetarListModel* model()
    {
        return __metars;
    }
    
public slots:
    void fetch(QString icao);
    void update();
    
protected:
    void timerEvent(QTimerEvent* event) override;
    
private slots:
    void __readMetars();
    
private:
    MetarListModel* __metars;
    PlainTextDownloader* __downloader;
    QQueue<QString> __requests;
    
};

#endif // METARUPDATER_H
