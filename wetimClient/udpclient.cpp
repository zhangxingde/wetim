#include <QHostAddress>

#include "udpclient.h"
#include "immessagchannel.h"

UdpClient::UdpClient(ImmesageChannel *mesgChannelPtr)
{
    mMesgChannelPtr = mesgChannelPtr;
    udpsock.bind(QAbstractSocket::DefaultForPlatform);
    connect(&udpsock, SIGNAL(readyRead()), mesgChannelPtr, SLOT(udpDataRecv()),Qt::DirectConnection);
}

int UdpClient::sendData(const void *p, unsigned int len, QString &dstaddr, int dstport)
{
    QHostAddress targeAddr;
    unsigned int slen = 0, r;

    targeAddr.setAddress(dstaddr);

    do {
        if ((r = udpsock.writeDatagram((const char*)p + slen, len - slen, targeAddr,dstport)) > 0){
            slen += r;
        }
    }while (r > 0 && udpsock.waitForBytesWritten());
    return slen;
}

int UdpClient::recvData(QByteArray &data, QString &peerAddr, int &peerPort)
{
    int len = 0;
    qint64 slen;
    quint16 port;
    QHostAddress srcaddr;

    while (udpsock.hasPendingDatagrams() && ((slen = udpsock.pendingDatagramSize()) > 0)){
        QByteArray d(slen, 0);

        udpsock.readDatagram(d.data(), d.length(), &srcaddr, &port);

        data += d;
    }
    peerAddr = srcaddr.toString();
    peerPort = port;
    return len;
}

void UdpClient::dataRecv()
{

}
