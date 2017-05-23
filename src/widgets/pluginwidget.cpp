#include "pluginwidget.h"
#include "ui_pluginwidget.h"
#include <QtWidgets>

namespace Vatsinator { namespace Widgets {

PluginWidget::PluginWidget(const QString &pluginId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginWidget),
    m_pluginId(pluginId)
{
    ui->setupUi(this);
    ui->pluginName->setText(QString());
    ui->pluginDescription->setText(QString());
    setExclusive(false);

    connect(ui->pluginEnabledCheckbox, &QAbstractButton::toggled, this, &PluginWidget::setEnabled);
    connect(ui->pluginEnabledRadio, &QAbstractButton::toggled, this, &PluginWidget::setEnabled);
}

PluginWidget::~PluginWidget()
{

}

QString PluginWidget::pluginName() const
{
    return m_pluginName;
}

void PluginWidget::setPluginName(const QString &pluginName)
{
    m_pluginName = pluginName;
    ui->pluginName->setText(pluginName);
}

QString PluginWidget::pluginDescription() const
{
    return m_pluginDescription;
}

void PluginWidget::setPluginDescription(const QString &pluginDescription)
{
    m_pluginDescription = pluginDescription;
    ui->pluginDescription->setText(pluginDescription);
}

bool PluginWidget::isEnabled() const
{
    return m_enabled;
}

void PluginWidget::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        ui->pluginEnabledCheckbox->setChecked(isEnabled());
        ui->pluginEnabledRadio->setChecked(isEnabled());
        emit toggled(enabled);
    }
}

bool PluginWidget::isExclusive() const
{
    return m_exclusive;
}

void PluginWidget::setExclusive(bool exclusive)
{
//    if (m_exclusive != exclusive) {
        m_exclusive = exclusive;
        ui->pluginEnabledCheckbox->setVisible(!m_exclusive);
        ui->pluginEnabledRadio->setVisible(m_exclusive);
//    }
}

QRadioButton *PluginWidget::radioButton()
{
    return ui->pluginEnabledRadio;
}

QString PluginWidget::pluginId() const
{
    return m_pluginId;
}

}} /* namespace Vatsinator::Widgets */
