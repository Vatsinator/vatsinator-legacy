/*
    toggleinboundoutboundlinesaction.h
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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


#ifndef TOGGLEINBOUNDOUTBOUNDLINESACTION_H
#define TOGGLEINBOUNDOUTBOUNDLINESACTION_H

#include <QAction>

class Airport;

/**
 * The action that corresponds to "Toggle inbout/outbound lines" option
 * for Airport right-mouse-button-click menu.
 * 
 * \todo Make this action actually works.
 */
class ToggleInboundOutboundLinesAction : public QAction {
  Q_OBJECT
  
signals:
  /**
   * Passed from QAction.
   * 
   * \param airport Airport for which the option was toggled.
   */
  void triggered(const Airport* airport);
  
public:
  /**
   * Creates new menu action for the specified _airport_.
   * The _parent_ param is passed to QAction's constructor.
   */
  ToggleInboundOutboundLinesAction(const Airport* airport, QObject* parent);
  
private:
  const Airport* __current;
  
};

#endif // TOGGLEINBOUNDOUTBOUNDLINESACTION_H
