/*
 * notifiable.h
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

#ifndef NOTIFIABLE_H
#define NOTIFIABLE_H

#include <QString>

/**
 * Notifiable is the base class for any classes that perform something that
 * should be spotted by the user. In other words, when something really
 * interesting happens (like a fatal error occurs) just call one of the
 * protected methods and don't worry about threads or dialog appearance.
 * This function will notify the UserInterface on its thread that the
 * dialog should pop up.
 */
class Notifiable {
  
protected:
  
  /**
   * Tell user that a fatal error occured.
   * This method will open an error dialog. You are likely to call
   * QCoreApplication::quit() just after that, as fatal errors mean that
   * the application just cannot continue its execution.
   * 
   * \param message Message to be shown in the error dialog.
   */
  void notifyError(const QString& message);
  
  /**
   * Tell user that something went really wrong.
   * This type of notification tells user that something here is not O.K.
   * (like a file is missing) and he should fix it (by reinstalling the
   * application, for example), but the application can continue,
   * as it is not the fatal error.
   * 
   * \param message Message to be shown in the warning dialog.
   */
  void notifyWarning(const QString& message);
  
  /**
   * Tell user something interesting that he should know about.
   * There is no error nor warning, just the coffee is ready.
   * 
   * \param message Message to be shown in the info dialog.
   */
  void notifyInfo(const QString& message);
  
  
};

#endif // NOTIFIABLE_H
