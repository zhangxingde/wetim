﻿#include "immessagchannel.h"
#include "tcpconnect.h"
#include "immessage_def.h"
#include "udpclient.h"
#include "clientsqldb.h"


ImmesageChannel::ImmesageChannel()
{
    ClientSqlDb *clientSqlDbPtr;

    isQuit = 0;
    udpClientPtr = new UdpClient(this);
    memset(imObsevers, 0 ,sizeof(imObsevers));
    memset(imObsevArg, 0, sizeof(imObsevArg));
    clientSqlDbPtr = ClientSqlDb::getInstance();

    setDstSevInaddrInfo(clientSqlDbPtr->queryLogonSevAddr(),clientSqlDbPtr->queryLogonSevTcport(),
                        clientSqlDbPtr->queryLogonSevUdport());
    clientSqlDbPtr->close();

    start();
}

ImmesageChannel::~ImmesageChannel()
{
    terminate();
    isQuit = 1;
    wait();
    delete messageQueuePtr;
    delete udpClientPtr;
    for (unsigned int i = 0; i < sizeof(imObsevers)/sizeof(imObsevers[0]); ++i){
        if (imObsevers[i])
            delete imObsevers[i];
    }    
}

unsigned int ImmesageChannel::sendTcpData2Server(const QString &addr, unsigned int tport, const ImmessageData &data)
{
    TcpClient tc, *mtcpsockptr = &tc;
    unsigned int len = 0;
    if (mtcpsockptr->connet2Server(addr, tport)){
        QByteArray remData;
        QString peerAddr;
        int peerPort;
        bool b;

        peerIpv4Addr = mtcpsockptr->getLocalAddr();
        len = mtcpsockptr->sendData(data.getDataPtr(), data.length());
        b = len > 0 && mtcpsockptr->recvData(remData, peerAddr, peerPort);
        mtcpsockptr->close();
        if (b){
            ImmessageData m;

            m.recvRawData(remData.constData(), remData.length());
            pushTcpDataIn(m, peerAddr, peerPort);
        }
    }else{
        emit sockerror(mtcpsockptr->getErrorNo(), mtcpsockptr->getErrorString());
    }
    return len;
}

bool ImmesageChannel::pushTcpDataOut(const ImmessageData &data, const QString &addr, int port)
{
    return messageQueuePtr->pushMesg(data, addr, port, 1, 1);
}

bool ImmesageChannel::pushTcpDataIn(const ImmessageData &data, const QString &addr, int port)
{
    return messageQueuePtr->pushMesg(data, addr, port, 0,1);
}

bool ImmesageChannel::pushUdpDataOut(const ImmessageData &data, const QString &addr, int port)
{
    if (port < 0 || !addr.length()){
        return messageQueuePtr->pushMesg(data, dstSevIpv4, dstUdpPort,1, 0);
    }
    return messageQueuePtr->pushMesg(data, addr, port,1, 0);
}

bool ImmesageChannel::pushUdpDataIn(const ImmessageData &data, const QString &addr, int port)
{
    return messageQueuePtr->pushMesg(data, addr, port, 0, 0);
}

void ImmesageChannel::udpDataRecv()
{
    QByteArray data;
    QString addr;
    int port;
    ImmessageData m;

    udpClientPtr->recvData(data,addr, port);
    m.recvRawData(data.constData(), data.length());
    pushUdpDataIn(m, addr, port);
}

bool ImmesageChannel::regOneImobsever(ImmesgObsev *obsever, void *p)
{
    int t = obsever->getMesgType();

    if (imObsevers[t])
        delete imObsevers[t];
    imObsevers[t] = obsever;
    imObsevArg[t] = p;
    return 1;
}

unsigned int ImmesageChannel::getLocalUdpIpv4()
{
    return peerIpv4Addr;
}

unsigned short ImmesageChannel::getLocalUdpProt()
{
    return udpClientPtr->getLOcalPort();
}

bool ImmesageChannel::publishAnImmessage(ImmessageData &src, QString &addr, int port)
{
    int t;

    if (!src.isValid()){
        return 0;
    }
    t = src.getMesgType();
    if (imObsevers[t]){
        imObsevers[t]->workIngWithRecvMessage(src, addr.toStdString().c_str(), port,imObsevArg[t]);
    }
    return 1;
}

void ImmesageChannel::setDstSevInaddrInfo(const char *ipv4, unsigned short tport, unsigned short uport)
{
    if (ipv4){
        strncpy(dstSevIpv4, ipv4, sizeof(dstSevIpv4));
    }
    dstTcpPort = tport;
    dstUdpPort = uport;
}

void ImmesageChannel::run()
{
    messageQueuePtr = new imMessageQueue_c(1024);
    imMessageQueue_c::MessageCell_t mcell;

    while (!isQuit){
        messageQueuePtr->getMesg(mcell.m, mcell.addr, mcell.port, mcell.dir, mcell.isTcp);
        if (mcell.dir){//out
            if (mcell.isTcp)
                sendTcpData2Server(mcell.addr, mcell.port, mcell.m);
            else
                udpClientPtr->sendData(mcell.m.getDataPtr(), mcell.m.length(), mcell.addr, mcell.port);
        }else{
            publishAnImmessage(mcell.m, mcell.addr, mcell.port);
        }
    }
}
