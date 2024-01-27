#include "device.hpp"
#include "interface.hpp"
#include "listener.hpp"
#include "timing.hpp"
#include <QQueue>
#include <QDate>
#include <QDataStream>
#include <QDebug>
#include "api/candle.h"

CandleApiDevice::CandleApiDevice(QString _name, QString dev_name, uint32_t ch)
{
    qDebug() << "New CandleApiDevice is created:" << _name << dev_name << ch;
    _iface = findInterface(dev_name);
    name = _name;
    channel = ch;
}

CandleApiDevice::~CandleApiDevice()
{
    qDebug() << name << "CandleApiDevice is destoyed.";
}

bool CandleApiDevice::open()
{
    setState(CanBusDeviceState::ConnectingState);
    qDebug() << name << "CandleApiDevice::open";

    if (!_iface->openChannel(channel))
    {
	    qDebug() << "!openChannel()";
	    setState(CanBusDeviceState::UnconnectedState);
	    return false;
    }

    qDebug() << name << "configurationKeys()";
    QVector<int> keys = configurationKeys();
    foreach (int key, keys)
    {
        if (key == QCanBusDevice::BitRateKey)
        {
            QVariant var = configurationParameter(key);
            bool ok;
            uint32_t bitrate = var.toInt(&ok);
            if (ok)
            {
		qDebug() << name << "setBitTiming()" << bitrate;
                if (!setBitTiming(bitrate, 875))
                {
                    qDebug() << name << "!setBitTiming()" << bitrate;
                    close();
                    return false;
                }
            }
        }
    }
   
    _iface->startChannel(channel);

    connect(_iface, &CandleApiInterface::sig_msg, this, &CandleApiDevice::slt_msg, Qt::QueuedConnection);

    setState(CanBusDeviceState::ConnectedState);
    return true;
}

void CandleApiDevice::close()
{
    setState(CanBusDeviceState::ClosingState);
    qDebug() << name << "CandleApiDevice::close";
    
    _iface->closeChannel(channel);

    setState(CanBusDeviceState::UnconnectedState);
}

bool CandleApiDevice::writeFrame(const QCanBusFrame &msg)
{
    candle_frame_t frame;
    frame.can_id = msg.frameId() & 0x1FFFFFFF; //??

    //qDebug() << "CandleApiDevice::writeFrame";
    switch (msg.frameType())
    {
    case QCanBusFrame::DataFrame:
        break;
    case QCanBusFrame::RemoteRequestFrame:
        frame.can_id |= CANDLE_ID_RTR;
        break;
    case QCanBusFrame::ErrorFrame:
        frame.can_id |= CANDLE_ID_ERR;
        break;
    default:
        qDebug() << "CandleApiDevice::writeFrame -> unknown or error frame.";
        return true;
    }

    if (msg.hasExtendedFrameFormat())
    {
        frame.can_id |= CANDLE_ID_EXTENDED;
    }

    frame.can_dlc = msg.payload().length();
    for (int cnt = 0; cnt < 8 && cnt < frame.can_dlc; cnt++)
    {
        frame.data[cnt] = msg.payload().at(cnt);
    }
    frame.channel = channel;

    return _iface->writeFrame(frame);
}

QString CandleApiDevice::interpretErrorFrame(const QCanBusFrame &errorFrame)
{
    Q_UNUSED(errorFrame);
    qDebug() << "CandleApiDevice::interpretErrorFrame";
    return QString();
}

bool CandleApiDevice::setBitTiming(uint32_t bitrate, uint32_t samplePoint)
{
    candle_handle handle = _iface->getHandle();

    candle_capability_t caps;
    if (!candle_channel_get_capabilities(handle, channel, &caps))
    {
        qDebug() << "CandleApiDevice::setBitTiming:" << "can't get caps";
        return false;
    }

    foreach (const CandleApiTiming t, _timings)
    {
        if ( (t.getBaseClk() == caps.fclk_can)
          && (t.getBitrate()==bitrate)
          && (t.getSamplePoint()==samplePoint) )
        {
            candle_bittiming_t timing = t.getTiming();
            return candle_channel_set_timing(handle, channel, &timing);
        }
    }

    qDebug() << "CandleApiDevice::setBitTiming:" << "no valid timing found";
    return false;
}

void CandleApiDevice::slt_msg(quint8 ch, const QCanBusFrame & msg)
{
    //qDebug() << "slt msg:" << ch << msg.toString();

    if (ch != channel)
        return;

    QVector<QCanBusFrame> msg_list;
    msg_list.append(msg);
    enqueueReceivedFrames(msg_list);
}

