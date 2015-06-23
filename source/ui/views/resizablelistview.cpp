/*
 * resizablelistview.cpp
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

#include "resizablelistview.h"

ResizableListView::ResizableListView(QWidget* parent): QListView(parent) {}

QSize
ResizableListView::sizeHint() const
{
    if (!model())
        return QSize(QListView::sizeHint().width(), 0);
        
    int h = 0;
    
    for (int i = 0; i < model()->rowCount(); ++i)
        h += sizeHintForRow(i);
        
    return QSize(QListView::sizeHint().width(), h);
}

void
ResizableListView::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    QListView::rowsAboutToBeRemoved(parent, start, end);
    updateGeometry();
}

void
ResizableListView::rowsInserted(const QModelIndex& parent, int start, int end)
{
    QListView::rowsInserted(parent, start, end);
    updateGeometry();
}
