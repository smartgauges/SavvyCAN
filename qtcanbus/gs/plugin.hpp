#pragma once

#include <QObject>
#include <QtSerialBus/qcanbus.h>
#include <QtSerialBus/qcanbusdevice.h>
#include <QtSerialBus/qcanbusfactory.h>
#include <QtSerialBus/qcanbusdeviceinfo.h>

class CandleApiInterface;

class CandleLightCanBusPlugin : public QObject, public QCanBusFactoryV2
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QCanBusFactory" FILE "plugin.json")
    Q_INTERFACES(QCanBusFactoryV2)

    friend class QCanBusDevice;

public:
    CandleLightCanBusPlugin();
    
    QList<QCanBusDeviceInfo> availableDevices(QString *errorMessage) const override;
    QCanBusDevice *createDevice(const QString &interfaceName, QString *errorMessage) const override;
};

Q_DECLARE_METATYPE(QCanBusFrame)

