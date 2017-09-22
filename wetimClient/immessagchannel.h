#ifndef IMMESSAGCHANNEL_H
#define IMMESSAGCHANNEL_H
#include <QString>
#include <QThread>
#include <QObject>
#include "include_h/singletontemp.h"
#include "immessagequeue.h"
#include "immesgdecor.h"

class ImmessageData;
class UdpClient;
class ImmesageChannel: public QThread, public SingletonTempBase<ImmesageChannel>
{
    Q_OBJECT
public:
    friend class SingletonTempBase<ImmesageChannel>;
    bool pushTcpDataOut (const ImmessageData &data, const QString &addr, int port);
    bool pushTcpDataIn (const ImmessageData &data, const QString &addr, int port);
    bool pushUdpDataOut (const ImmessageData &data, const QString &addr, int port);
    bool pushUdpDataIn (const ImmessageData &data, const QString &addr, int port);

    bool regOneImobsever (ImmesgObsev *obsever, void *p);

public slots:
    void udpDataRecv ();

signals:
    void sockerror(int socketError, const QString &message);


private:
    ImmesageChannel();
    ~ImmesageChannel();
    imMessageQueue_c *messageQueuePtr;
    UdpClient *udpClientPtr;

    int isQuit;
    ImmesgObsev *imObsevers[__IMMESG_MAX_NUM];
    void *imObsevArg[__IMMESG_MAX_NUM];

    void run();
    unsigned int sendTcpData2Server(const QString &addr, unsigned int tport, const ImmessageData &data);
    bool publishAnImmessage(ImmessageData &src, QString &addr, int port);


};


#endif
