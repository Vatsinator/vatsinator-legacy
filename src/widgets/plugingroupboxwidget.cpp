#include "plugingroupboxwidget.h"
#include <QtWidgets>

namespace Vatsinator { namespace Widgets {

PluginGroupBoxWidget::PluginGroupBoxWidget(QWidget* parent) :
    QGroupBox(parent),
    m_buttonGroup(new QButtonGroup)
{
    QVBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);
}

PluginGroupBoxWidget::~PluginGroupBoxWidget()
{

}

void PluginGroupBoxWidget::addPluginWidget(PluginWidget* pluginWidget)
{
    if (layout()->count() < 0) {
        QFrame* frame = new QFrame;
        frame->setFrameShape(QFrame::HLine);
    }

    layout()->addWidget(pluginWidget);

    pluginWidget->setExclusive(true);
    m_buttonGroup->addButton(pluginWidget->radioButton());

    connect(pluginWidget, &PluginWidget::toggled, this, &PluginGroupBoxWidget::onPluginToggled);
}

void PluginGroupBoxWidget::setSelectedPlugin(const QString& pluginId)
{
    for (int i = 0; i < layout()->count(); ++i) {
        PluginWidget* w = qobject_cast<PluginWidget*>(layout()->itemAt(i)->widget());
        if (w->pluginId() == pluginId) {
            w->radioButton()->setChecked(true);
            m_selectedPlugin = pluginId;
            emit selectedPluginChanged(m_selectedPlugin);
            return;
        }
    }
}

void PluginGroupBoxWidget::onPluginToggled(bool enabled)
{
    if (enabled) {
        m_selectedPlugin = qobject_cast<PluginWidget*>(sender())->pluginId();
        emit selectedPluginChanged(m_selectedPlugin);
    }
}

}} /* namespace Vatsinator::Widgets */
