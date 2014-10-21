/*
 * decisionevent.h
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

#ifndef DECISIONEVENT_H
#define DECISIONEVENT_H

#include <QEvent>

#include "events/types.h"

/**
 * The DecisionEvent is used whenever the user makes a decision and therefore
 * appropriate action should be taken.
 */
class DecisionEvent : public QEvent {
  
public:
  /**
   * The Decision enum describes user decisions.
   */
  enum Decision {
    Accepted,   /**< User pressed "OK", "Accept" or something that indicates
                  a positive decision. */
    TryAgain,   /**< User pressed "Try again" button. */
    Declined    /**< User pressed "No", "Decline" or something that indicates
                  a negative decision. */
  };
  
  /**
   * Creates a new DecisionEvent.
   * 
   * \param context Describes a context where the particular decision was made.
   * \param decision The decision that has been made.
   */
  DecisionEvent(QString context, Decision decision);
  
  /**
   * Gets the decision context.
   */
  inline const QString& context() const { return __context; }
  
  /**
   * Gets the user decision.
   */
  inline Decision decision() const { return __decision; }
  
private:
  QString __context;
  Decision __decision;
  
};

#endif // DECISIONEVENT_H
