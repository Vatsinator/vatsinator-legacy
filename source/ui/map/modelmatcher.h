/*
 * modelmatcher.h
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

#ifndef MODELMATCHER_H
#define MODELMATCHER_H

#include <QObject>
#include "ui/notifiable.h"

class ModelMatcher : public QObject, private Notifiable {
    Q_OBJECT

public:
    ModelMatcher(QObject* parent = nullptr);
    
    QPixmap& match(const QString& acft);

private:
    void __readModels();
    void __loadPixmaps();
    
private:
    /*
     * Aircraft <-> id pairs
     */
    QList<QPair<QString, QString>> __modelsIds;
    
    /*
     * Id <-> texture pairs
     */
    QMap<QString, QPixmap> __modelsPixmaps;
};

#endif // MODELMATCHER_H
