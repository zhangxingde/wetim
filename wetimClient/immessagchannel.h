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
    bool pushUdpDataOut (const ImmessageData &data, const QString &addr = QString(), int port = -1);
    bool pushUdpDataIn (const ImmessageData &data, const QString &addr, int port);

    bool regOneImobsever (ImmesgObsev *obsever, void *p);
    unsigned int getLocalUdpIpv4 ();
    unsigned short getLocalUdpProt ();

public slots:
    void udpDataRecv ();

signals:
    void sockerror(int socketError, const QString &message);


private:
    ImmesageChannel();
    ~ImmesageChannel();
    imMessageQueue_c *messageQueuePtr;
    UdpClient *udpClientPtr;


    unsigned int peerIpv4Addr;
    char dstSevIpv4[32];
    unsigned short dstTcpPort;
    unsigned short dstUdpPort;

    int isQuit;
    ImmesgObsev *imObsevers[__IMMESG_MAX_NUM];
    void *imObsevArg[__IMMESG_MAX_NUM];

    void setDstSevInaddrInfo (const char *ipv4, unsigned short tport, unsigned short uport);
    void run();
    unsigned int sendTcpData2Server(const QString &addr, unsigned int tport, const ImmessageData &data);
    bool publishAnImmessage(ImmessageData &src, QString &addr, int port);


};


#endif
