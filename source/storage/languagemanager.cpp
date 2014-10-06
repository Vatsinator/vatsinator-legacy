/*
    languagemanager.cpp
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

#include <QtCore>

#include "storage/filemanager.h"

#include "languagemanager.h"

#include "vatsinatorapplication.h"

LanguageManager::LanguageManager() {
  __getInstalledLanguages();
}

QStringList
LanguageManager::allLanguages() {
  QStringList result;
  for (LangInfo& li: __languages)
    result << li.description;
  
  return result;
}

int
LanguageManager::getLanguageId(const QString& locale) {
  for (LangInfo& li: __languages)
    if (li.locale == locale)
      return li.id;
  
  return 0;
}

const QString&
LanguageManager::getLocaleById(int id) {
  return __languages[id].locale;
}

void
LanguageManager::__getInstalledLanguages() {
  int currentId = 0;
  
  QDir translationsDir(FileManager::staticPath(FileManager::Translations));
  QStringList locales = translationsDir.entryList({"vatsinator-*.qm"});
  QStringList descriptions = translationsDir.entryList({"*.language"});
  
  for (const QString& l: locales) {
    QString locale = l.section('-', 1, 1).section('.', 0, 0);
    if (!descriptions.contains(locale + ".language")) {
      qCritical("LanguageManager: could not find description for %s.", qPrintable(locale));
    }
    
    QFile descFile(FileManager::staticPath(FileManager::Translations) % "/" % locale % ".language");
    if (!descFile.open(QIODevice::ReadOnly | QIODevice::Text))
      continue;
    
    __languages.push_back({currentId, locale, QString::fromUtf8(descFile.readLine().simplified())});
    
    descFile.close();
    
    currentId += 1;
  }
}

