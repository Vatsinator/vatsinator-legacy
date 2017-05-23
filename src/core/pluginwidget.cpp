#include "pluginwidget.h"
#include "ui_pluginwidget.h"

namespace Vatsinator {
namespace Widgets {

PluginWidget::PluginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginWidget)
{
    ui->setupUi(this);
}

PluginWidget::~PluginWidget()
{
    delete ui;
}

} // namespace Widgets
} // namespace Vatsinator
