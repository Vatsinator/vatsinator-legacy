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
#include <misc/option.h>
#include <misc/pluginfinder.h>
#include <QtWidgets>

using namespace Vatsinator::Misc;

// loaded translators
static QList<QTranslator*> translators;


static void initTranslations(QApplication* app)
{
    auto loadTranslation = [](const QVariant& language) {
        Q_ASSERT(language.canConvert<QString>());
        qDebug() << "Switching language to" << language.toString();

        QLocale locale(language.toString());
        QLocale::setDefault(locale);

        std::for_each(translators.begin(), translators.end(),
                      std::bind(&QCoreApplication::removeTranslator, std::placeholders::_1));

        translators.clear();

        QTranslator* qttr = new QTranslator(qApp);
        if (qttr->load(locale, QStringLiteral("qt"), "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
            QCoreApplication::installTranslator(qttr);
            translators.append(qttr);
        }

        QTranslator* vatsinatortr = new QTranslator(qApp);
        if (vatsinatortr->load(locale, QStringLiteral("vatsinator"), "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath))
                || vatsinatortr->load(locale, QStringLiteral("vatsinator"), "_", QStringLiteral(VATSINATOR_I18N_PATH))) {
            QCoreApplication::installTranslator(vatsinatortr);
            translators.append(vatsinatortr);
        }
    };

    Option* language = new Option("misc/language", "en", app);
    QObject::connect(language, &Option::valueChanged, loadTranslation);
    loadTranslation(language->value());
}

static void initPluginFinder(QApplication* app)
{
    // fill list of directories where plugins may be found
    QStringList directories = { QCoreApplication::applicationDirPath() % "/plugins" };

#ifdef Q_OS_MACOS
    locations.append(QDir::cleanPath(QCoreApplication::applicationDirPath() % "/../PlugIns"));
    locations.append(QDir::cleanPath(QCoreApplication::applicationDirPath() % "/../../../plugins"));
#endif

#ifdef Q_OS_ANDROID
    /* This is a hacky way, but I couldn't find better */
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first());
    dir.cdUp();
    locations.append(dir.absolutePath() % "/qt-reserved-files/plugins");
#endif

    PluginFinder* pf = new PluginFinder(directories);
    QObject::connect(app, &QGuiApplication::aboutToQuit, [pf]() { delete pf; });
    app->setProperty("pluginFinder", QVariant::fromValue<PluginFinder*>(pf));
}

int main(int argc, char** argv)
{
    QCoreApplication::setApplicationName(QStringLiteral("Vatsinator"));
    QCoreApplication::setOrganizationName(QStringLiteral("VatsinatorTeam"));
    QCoreApplication::setApplicationVersion(QStringLiteral(VATSINATOR_VERSION));
    QCoreApplication::setOrganizationDomain(QStringLiteral("org.eu.vatsinator"));
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    qsrand(QTime::currentTime().msec());
    
    QApplication app(argc, argv);
    initPluginFinder(&app);
    initTranslations(&app);
    
    app.setWindowIcon(QIcon(QStringLiteral(":/icons/vatsinator.png")));
    VatsinatorWindow w;
    w.show();
    
    return app.exec();
}
