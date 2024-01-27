#pragma once

#include <QtSerialBus/QCanBusDevice>
#include "devices.hpp"

class CandleApiListener;

class CandleApiDevice : public CandleApiDevices
{
Q_OBJECT

public:
    explicit CandleApiDevice(QString name, QString dev_name, uint32_t channel);
    virtual ~CandleApiDevice();

public:
    bool open() override;
    void close() override;
    bool writeFrame(const QCanBusFrame &msg) override;
    QString interpretErrorFrame(const QCanBusFrame &errorFrame) override;

    bool setBitTiming(uint32_t bitrate, uint32_t samplePoint);
    void readFrames();

private slots:
    void slt_msg(quint8 channel, const QCanBusFrame & msg);

private:
    CandleApiInterface *_iface;
    QString name;
    uint32_t channel;
};
