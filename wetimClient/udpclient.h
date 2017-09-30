#ifndef UDPCLIENT_H
#define UDPCLIENT_H
#include <QObject>
#include <QUdpSocket>
#include <QDebug>

class ImmesageChannel;
class UdpClient:public QObject
{
    Q_OBJECT
public:
    UdpClient(ImmesageChannel *mesgChannelPtr);
    int sendData (const void *p, unsigned int len, QString &dstaddr, int dstport);
    int recvData(QByteArray &data, QString &peerAddr, int &peerPort);
    unsigned int getLocalIpv4 ()
    {
        return udpsock.localAddress().toIPv4Address();
    }
    unsigned short getLOcalPort () {return udpsock.localPort();}
private slots:
    void dataRecv ();
private:
    QUdpSocket udpsock;
    ImmesageChannel *mMesgChannelPtr;
};

#endif // UDPCLIENT_H
