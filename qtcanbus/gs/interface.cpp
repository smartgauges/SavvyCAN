#include <QDateTime>
#include <QDebug>

#include "interface.hpp"
#include "listener.hpp"

CandleApiInterface::CandleApiInterface(candle_handle handle, QString name) :
    _handle(handle),
    _name(name),
    channels(0),
    _lstn(0)
{
    _hostOffsetStart = QDateTime::currentMSecsSinceEpoch();
}

CandleApiInterface::~CandleApiInterface()
{
}

std::wstring CandleApiInterface::getPath() const
{
    return std::wstring(candle_dev_get_path(_handle));
}

QString CandleApiInterface::getName() const
{
    return _name;
}

candle_handle CandleApiInterface::getHandle() const
{
    return _handle;
}

bool CandleApiInterface::exists() const
{
    return candle_dev_exists(_handle);
}

bool CandleApiInterface::openChannel(uint8_t channel)
{
    if (!channels) {

	if (!candle_dev_open(_handle))
		return false;

	uint32_t t_dev;
	if (candle_dev_get_timestamp_us(_handle, &t_dev))
	{
		_hostOffsetStart = QDateTime::currentMSecsSinceEpoch();
		_deviceTicksStart = t_dev;
	}
    }

    channels |= (1 << channel);

    return true;
}

bool CandleApiInterface::startChannel(uint8_t channel)
{
    uint32_t flags = 0;
    candle_channel_start(_handle, channel, flags);

    if (!_lstn) {

	qDebug() << "create Listener";	   
        _lstn = new CandleApiListener(*this);

	_lstn->startThread();
    }

    return true;
}

void CandleApiInterface::closeChannel(uint8_t channel)
{
    candle_channel_stop(_handle, channel);

    channels &= ~(1 << channel);

    if (!channels) {
        _lstn->stopThread();
	delete _lstn;
	_lstn = 0;
	qDebug() << "before candle_dev_close()";
	candle_dev_close(_handle);
        qDebug() << "before candle_dev_close()";
    }
}

bool CandleApiInterface::writeFrame(candle_frame_t & frame)
{
    return candle_frame_send(_handle, frame.channel, &frame);
}

void CandleApiInterface::readFrames()
{
    candle_frame_t frame;
    unsigned int timeout_ms = 10;

    while (_lstn->_shouldBeRunning)
    {
        if (candle_frame_read(_handle, &frame, timeout_ms))
        {
            if (candle_frame_type(&frame) == CANDLE_FRAMETYPE_RECEIVE)
            {
                QCanBusFrame msg;

                if (candle_frame_is_rtr(&frame))
                {
                    msg.setFrameType(QCanBusFrame::RemoteRequestFrame);
                }
                else
                {
                    msg.setFrameType(QCanBusFrame::DataFrame);
                }
                if (candle_frame_is_extended_id(&frame))
                {
                    msg.setExtendedFrameFormat(true);
                }
                else
                {
                    msg.setExtendedFrameFormat(false);
                }
                msg.setFrameId(candle_frame_id(&frame) & 0x1FFFFFFF); //??

                uint8_t dlc = candle_frame_dlc(&frame);
                QByteArray payload = QByteArray(reinterpret_cast<const char*>(candle_frame_data(&frame)), dlc);
                msg.setPayload(payload);

                uint32_t dev_ts = candle_frame_timestamp_us(&frame) - _deviceTicksStart;
                uint64_t ts_us = _hostOffsetStart + dev_ts;

		uint64_t us_since_start = QDateTime::currentMSecsSinceEpoch();
                if (us_since_start > 0x180000000)
                {
                    // device timestamp overflow
                    ts_us += us_since_start & 0xFFFFFFFF00000000;
                }
                auto ts = QCanBusFrame::TimeStamp(ts_us/1000000, ts_us % 1000000);
                msg.setTimeStamp(ts);

		//qDebug() << "snd frame:" << msg.toString();
                emit sig_msg(frame.channel, msg);
            }
        }
    }
}

