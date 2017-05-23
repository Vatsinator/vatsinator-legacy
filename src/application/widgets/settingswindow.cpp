/*
 * settingswindow.cpp
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
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

#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "core/option.h"
#include "core/pluginfinder.h"
#include "gui/mapdrawerplugin.h"
#include "widgets/pluginwidget.h"
#include "config.h"
#include <QtWidgets>
#include <functional>

#ifdef Q_OS_MACOS
# include <QtMacExtras/QtMacExtras>
#endif

using namespace Vatsinator::Core;
using namespace Vatsinator::Gui;
using namespace Vatsinator::Widgets;

SettingsWindow::SettingsWindow(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    fillLanguages();
    fillPlugins();
    
#ifdef Q_OS_MACOS
    // adjust the window so it looks native on MacOS
    macosFixup();
#endif

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &SettingsWindow::handleButton);
    
    Option* statistics = new Option("misc/statistics", this);
    statistics->track(ui->statistics, "checked");
    
    Option* language = new Option("misc/language", this);
    /* Track the QComboBox's currentData property, but there is no notifier signal for it */
    connect(ui->language, QOverload<int>::of(&QComboBox::currentIndexChanged), [language, this](int index) {
        QString locale = this->ui->language->itemData(index).toString();
        language->setValue(locale);
    });

    QString locale = language->value().toString();
    int index = ui->language->findData(locale);
    ui->language->setCurrentIndex(index);

    Option* mapPlugin = new Option("plugins/map_drawer", this);
    mapPlugin->track(ui->mapDrawers, "selectedPlugin");

    m_mapAddons = new Option("plugins/map_addons", this);
    
    m_options << statistics << language << mapPlugin << m_mapAddons;
    
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
}

SettingsWindow::~SettingsWindow() {}

void SettingsWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(),
                                    QDesktopWidget().screenGeometry(QApplication::activeWindow())));
    
#ifndef Q_OS_MACOS
    std::for_each(m_options.begin(), m_options.end(), std::bind(&Option::setBlocked, std::placeholders::_1, true));
#endif
}

void SettingsWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        event->accept();

#ifdef Q_OS_MACOS
        ui->stackedWidget->currentWidget()->adjustSize();
        ui->stackedWidget->adjustSize();
        adjustSize();
#endif
    } else {
        event->ignore();
    }
}

void SettingsWindow::fillLanguages()
{
    QDir dir(QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QStringList trs = dir.entryList({ "vatsinator_*.qm" });
    for (auto tr: trs) {
        QString locale = tr;
        locale.truncate(locale.lastIndexOf('.'));
        locale.remove(0, locale.indexOf('_') + 1);
        
        QString lang = QLocale::languageToString(QLocale(locale).language());
        ui->language->addItem(lang, locale);
    }
}

void SettingsWindow::fillPlugins()
{
    QStringList mapPlugins = PluginFinder::pluginsForIid(qobject_interface_iid<MapDrawerPlugin*>());

    for (const QString& p: qAsConst(mapPlugins)) {
        QJsonObject metaData = PluginFinder::pluginMetaData(p);
        QString pluginName = metaData.contains("name") ? metaData.value("name").toString() : p;

        PluginWidget* pw = new PluginWidget(p);
        pw->setPluginName(pluginName);

        if (metaData.contains("description")) {
            pw->setPluginDescription(metaData.value("description").toString());
        }

        ui->mapDrawers->addPluginWidget(pw);
    }

    QVBoxLayout* layout = new QVBoxLayout;
    QStringList mapAddons = PluginFinder::pluginsForIid(qobject_interface_iid<MapAddon*>());

    QScopedPointer<Option> mapAddonsOption(new Option("plugins/map_addons", this));
    QStringList enabledMapAddons = mapAddonsOption->value().toStringList();

    for (const QString& p: qAsConst(mapAddons)) {
        QJsonObject metaData = PluginFinder::pluginMetaData(p);
        QString pluginName = metaData.contains("name") ? metaData.value("name").toString() : p;

        PluginWidget* pw = new PluginWidget(p);
        pw->setPluginName(pluginName);

        if (metaData.contains("description")) {
            pw->setPluginDescription(metaData.value("description").toString());
        }

        pw->setEnabled(enabledMapAddons.contains(p));
        layout->addWidget(pw);

        connect(pw, &PluginWidget::toggled, this, &SettingsWindow::handlePluginToggled);
    }

    ui->mapAddons->setLayout(layout);
}

#ifdef Q_OS_MACOS
void SettingsWindow::macosFixup()
{
    setMinimumWidth(400);

    QMacToolBar* macTb = new QMacToolBar(this);
    QSignalMapper* mapper = new QSignalMapper(this);
    connect(mapper, qOverload<int>(&QSignalMapper::mapped), ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    auto adjustSizeToWidgetNo = [this](int n) {
        for (int i = 0; i < ui->stackedWidget->count(); ++i) {
            QWidget* w = ui->stackedWidget->widget(i);
            QSizePolicy::Policy policy = i == n ? QSizePolicy::Preferred : QSizePolicy::Ignored;
            w->setSizePolicy(policy, policy);
            w->adjustSize();
        }

        ui->stackedWidget->adjustSize();
        adjustSize();
    };

    connect(mapper, qOverload<int>(&QSignalMapper::mapped), [macTb, adjustSizeToWidgetNo, this](int i) {
        QMacToolBarItem* item = macTb->items().at(i);
        setWindowTitle(item->text());

        adjustSizeToWidgetNo(i);
    });

    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem* item = ui->listWidget->item(i);
        QMacToolBarItem* mtbi = macTb->addItem(item->icon(), item->text());
        mtbi->setSelectable(true);
        connect(mtbi, &QMacToolBarItem::activated, mapper, qOverload<>(&QSignalMapper::map));
        mapper->setMapping(mtbi, i);

        QWidget* page = ui->stackedWidget->widget(i);
        page->setContentsMargins(0, 0, 0, 0);
        page->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }

    setWindowTitle(macTb->items().at(0)->text());
    setWindowIcon(QIcon());
    setContentsMargins(0, 0, 0, 0);

    ui->gridLayout->removeWidget(ui->listWidget);
    ui->listWidget->setVisible(false);
    ui->gridLayout->removeWidget(ui->buttonBox);
    ui->buttonBox->setVisible(false);

    adjustSizeToWidgetNo(0);

    window()->winId();
    macTb->attachToWindow(window()->windowHandle());

    QFont font = ui->statsLabel->font();
    font.setPointSize(font.pointSize() - 1);
    ui->statsLabel->setFont(font);
}
#endif

void SettingsWindow::handleButton(QAbstractButton* button)
{
    if (ui->buttonBox->button(QDialogButtonBox::Apply) == button) {
        // TODO Use Option::commit() someday maybe?
        std::for_each(m_options.begin(), m_options.end(), std::bind(&Option::setBlocked, std::placeholders::_1, false));
        std::for_each(m_options.begin(), m_options.end(), std::bind(&Option::setBlocked, std::placeholders::_1, true));
    } else if (ui->buttonBox->button(QDialogButtonBox::Ok) == button) {
        std::for_each(m_options.begin(), m_options.end(), std::bind(&Option::setBlocked, std::placeholders::_1, false));
        close();
    } else if (ui->buttonBox->button(QDialogButtonBox::Cancel) == button) {
        // TODO Revert
        close();
    }
}

void SettingsWindow::handlePluginToggled()
{
    PluginWidget* pw = qobject_cast<PluginWidget*>(sender());
    Q_ASSERT(pw);

    QStringList enabledMapAddons = m_mapAddons->value().toStringList();

    if (!pw->isEnabled())
        enabledMapAddons.removeAll(pw->pluginId());
    else
        enabledMapAddons.append(pw->pluginId());

    qDebug() << enabledMapAddons;

    m_mapAddons->setValue(QVariant::fromValue(enabledMapAddons));
}
