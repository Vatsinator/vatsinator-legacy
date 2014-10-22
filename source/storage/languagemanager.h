/*
    languagemanager.h
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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


#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QStringList>
#include <QVector>

#include "singleton.h"

/**
 * The LanguageManager class keeps track of translations installed on
 * the system.
 */
class LanguageManager : public Singleton<LanguageManager> {
  
public:
  /**
   * The constructor looks for installed languages in Vatsinator
   * standard locations.
   * On Linux it is <prefix>/share/vatsinator/translations/
   * On Windows it is C:/Program Files/Vatsinator/translations/
   * On MacOS it is Vatsinator.app/Contents/Resources/translations
   */
  LanguageManager();
  
  /**
   * Gets list of all installed languages.
   * Each language is described by its .language file stored in
   * the translations directory.
   */
  QStringList allLanguages();
  
  /**
   * Gets language unique ID.
   * 
   * \param locale The language locale (e.g. pt, ja_JP).
   * \sa getLocaleById().
   */
  int getLanguageId(const QString& locale);
  
  /**
   * Gets language name by the specific id.
   * 
   * \param id The language unique id.
   * \sa getLanguageId().
   */
  const QString& getLocaleById(int id);
  
private:
  struct LangInfo {
    int         id;
    QString     locale;
    QString     description;
  };
  
  void __getInstalledLanguages();
  
  QVector<LangInfo> __languages;
  
};

#endif // LANGUAGEMANAGER_H
