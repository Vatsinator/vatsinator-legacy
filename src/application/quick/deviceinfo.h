#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QObject>

class DeviceInfo  : public QObject {
    Q_OBJECT

    Q_PROPERTY(int navigationBarHeight READ navigationBarHeight)

public:
    DeviceInfo(QObject* parent = nullptr);

    int navigationBarHeight();

private:
    int m_navigationBarHeight = -1;

};

#endif // DEVICEINFO_H
