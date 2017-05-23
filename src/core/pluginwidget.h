#ifndef VATSINATOR_WIDGETS_PLUGINWIDGET_H
#define VATSINATOR_WIDGETS_PLUGINWIDGET_H

#include <QWidget>

namespace Vatsinator {
namespace Widgets {

namespace Ui {
class PluginWidget;
}

class PluginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PluginWidget(QWidget *parent = 0);
    ~PluginWidget();

private:
    Ui::PluginWidget *ui;
};


} // namespace Widgets
} // namespace Vatsinator
#endif // VATSINATOR_WIDGETS_PLUGINWIDGET_H
