/*
    languagemanager.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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

#include <QtCore>

#include "languagemanager.h"
#include "defines.h"

#include "vatsinatorapplication.h"

LanguageManager::LanguageManager() {
  __languages.push_back({0, "en_GB", "English"});
  
  __getInstalledLanguages();
}

QStringList
LanguageManager::getAllLanguages() {
  QStringList result;
  for (LangInfo& li: __languages)
    result << li.description;
  
  return result;
}

int
LanguageManager::getLanguageId(const QString& _locale) {
  for (LangInfo& li: __languages)
    if (li.locale == _locale)
      return li.id;
  
  return 0;
}

const
QString& LanguageManager::getLocaleById(int _id) {
  return __languages[_id].locale;
}

void
LanguageManager::__getInstalledLanguages() {
  int currentId = 1;
  
  QDir translationsDir(TRANSLATIONS_DIR);
  QStringList locales = translationsDir.entryList({"vatsinator-*.qm"});
  QStringList descriptions = translationsDir.entryList({"*.language"});
  
  for (const QString& l: locales) {
    QString locale = l.section('-', 1, 1).section('.', 0, 0);
    if (!descriptions.contains(locale + ".language")) {
      VatsinatorApplication::log("Could not find description for %s.", locale.toStdString().c_str());
    }
    
    QFile descFile(static_cast< QString >(TRANSLATIONS_DIR) + "/" + locale + ".language");
    if (!descFile.open(QIODevice::ReadOnly | QIODevice::Text))
      continue;
    
    __languages.push_back({currentId, locale, descFile.readLine().simplified()});
    
    descFile.close();
    
    currentId += 1;
  }
}

