/*
    main.cpp
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

#include "vatsinatorwindow.h"
#include "config.h"
#include "core/option.h"
#include <QtWidgets>

using namespace Vatsinator::Core;

int main(int argc, char** argv)
{
    QCoreApplication::setApplicationName(QStringLiteral("Vatsinator"));
    QCoreApplication::setOrganizationName(QStringLiteral("VatsinatorTeam"));
    QCoreApplication::setApplicationVersion(QStringLiteral(VATSINATOR_VERSION));
    QCoreApplication::setOrganizationDomain(QStringLiteral("org.eu.vatsinator"));
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    qsrand(QTime::currentTime().msec());
    
    QApplication app(argc, argv);
    QList<QTranslator*> translators;

    auto loadTranslation = [&translators](const QString& language) {
        qDebug() << "Switching language to" << language;

        QLocale locale(language);
        QLocale::setDefault(locale);

        std::for_each(translators.begin(), translators.end(),
                      std::bind(&QCoreApplication::removeTranslator, std::placeholders::_1));

        static QList<QString> prefixes = {
            QStringLiteral("qt"), QStringLiteral("qtbase"),
            QStringLiteral("vatsinator")
        };

        for (const QString& prefix: qAsConst(prefixes)) {
            QTranslator* tr = new QTranslator(qApp);
            if (tr->load(locale, prefix, "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
                qDebug() << "Loaded translation:" << prefix << locale;
                QCoreApplication::installTranslator(tr);
                translators << tr;
            }
        }
    };
    
    Option* language = new Option("misc/language", "en", &app);
    QObject::connect(language, &Option::valueChanged, [loadTranslation](const QVariant& value) {
        QString language = value.toString();
        loadTranslation(language);
    });

    loadTranslation(language->value().toString());
    
    app.setWindowIcon(QIcon(QStringLiteral(":/icons/vatsinator.png")));
    VatsinatorWindow w;
    w.show();
    
    return app.exec();
}
