#pragma once

#include <QObject>
#include <QCanBusFrame>
#include "api/candle.h"

class CandleApiDevices;
class CandleApiListener;

class CandleApiInterface: public QObject
{
Q_OBJECT

public:
    CandleApiInterface(candle_handle handle, QString name);
    virtual ~CandleApiInterface();

    std::wstring getPath() const;
    QString getName() const;
    candle_handle getHandle() const;
    bool exists() const;

    bool openChannel(uint8_t channel);
    bool startChannel(uint8_t channel);
    void closeChannel(uint8_t channel);
    void readFrames();
    bool writeFrame(candle_frame_t & frame);

private:
    candle_handle _handle;
    QString _name;
    uint32_t channels;
    CandleApiListener *_lstn;
    uint32_t _deviceTicksStart;
    uint64_t _hostOffsetStart;

signals:
    void sig_msg(quint8 channel, const QCanBusFrame & frame);
};
