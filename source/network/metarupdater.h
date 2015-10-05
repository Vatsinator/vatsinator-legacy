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
class Metar;
class Icao;

/**
 * The MetarUpdater class is responsible for fetching new weather reports
 * and updating the existing ones.
 */
class MetarUpdater : public QObject {
    Q_OBJECT
    
    /**
     * The model that contains all fetched weather reports.
     */
    Q_PROPERTY(MetarListModel* model READ model CONSTANT)
    
public:
    MetarUpdater(MetarListModel* model, QObject* parent = nullptr);
    
    inline MetarListModel* model()
    {
        return __metars;
    }
    
public slots:
    /**
     * Starts downloading the METAR, if it is not fetched yet.
     * \todo update doc
     */
    Metar* fetch(const QString& request);
    
    /**
     * Updates all the weather reports.
     */
    void update();
    
protected:
    void timerEvent(QTimerEvent* event) override;
    
private:
    void __update(const QString& metar);
    
private slots:
    void __readMetars();
    
private:
    MetarListModel* __metars;
    PlainTextDownloader* __downloader;
    QQueue<QString> __requests;
    
};

#endif // METARUPDATER_H
