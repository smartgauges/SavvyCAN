#include "listener.hpp"
#include "interface.hpp"
#include <QThread>
#include <QtCore/QList>
#include <QtSerialBus/QCanBusDevice>
#include <QDebug>

CandleApiListener::CandleApiListener(CandleApiInterface &iface) :
    _shouldBeRunning(false),
    _iface(iface)
{
    _thread = new QThread();
}

CandleApiListener::~CandleApiListener()
{
    delete _thread;
}

void CandleApiListener::run()
{
    _iface.readFrames();
    _thread->quit();
}

void CandleApiListener::startThread()
{
    _shouldBeRunning = true;
    moveToThread(_thread);
    connect(_thread, SIGNAL(started()), this, SLOT(run()));
    _thread->start();
}

void CandleApiListener::stopThread()
{
    qDebug() << "stopThread:" << _shouldBeRunning;
    if (_shouldBeRunning)
    {
        _shouldBeRunning = false;
        _thread->wait();
    }

    qDebug() << "stopThread: exit";
}

