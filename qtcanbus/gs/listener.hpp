#pragma once

#include <QObject>
#include <QCanBusFrame>

class CandleApiInterface;

class CandleApiListener : public QObject
{
    Q_OBJECT

public:
    explicit CandleApiListener(CandleApiInterface &dev);
    virtual ~CandleApiListener();

public slots:
    void run();
    void startThread();
    void stopThread();
 
public:
    bool _shouldBeRunning;

private:
    CandleApiInterface &_iface;
    QThread *_thread;
};
