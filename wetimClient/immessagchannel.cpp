#include "immessagchannel.h"
#include "tcpconnect.h"
#include "immessage_def.h"


ImmesageChannel::ImmesageChannel()
{
    isQuit = 0;
    memset(imObsevers, 0 ,sizeof(imObsevers));
    memset(imObsevArg, 0, sizeof(imObsevArg));
    start();
}

ImmesageChannel::~ImmesageChannel()
{
    terminate();
    isQuit = 1;
    wait();
    delete messageQueuePtr;
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

bool ImmesageChannel::regOneImobsever(ImmesgObsev *obsever, void *p)
{
    int t = obsever->getMesgType();

    if (imObsevers[t])
        delete imObsevers[t];
    imObsevers[t] = obsever;
    imObsevArg[t] = p;
    return 1;
}

bool ImmesageChannel::publishAnImmessage(ImmessageData &src)
{
    int t;

    if (!src.isValid()){
        return 0;
    }
    t = src.getMesgType();
    if (imObsevers[t]){
        imObsevers[t]->workIngWithRecvMessage(src, imObsevArg[t]);
    }
    return 1;
}

void ImmesageChannel::run()
{
    messageQueuePtr = new imMessageQueue_c(1024);
    imMessageQueue_c::MessageCell_t mcell;

    while (!isQuit){
        messageQueuePtr->getMesg(mcell.m, mcell.addr, mcell.port, mcell.dir, mcell.isTcp);
        if (mcell.dir){
            if (mcell.isTcp)
                sendTcpData2Server(mcell.addr, mcell.port, mcell.m);
        }else{
            publishAnImmessage(mcell.m);
        }
    }
}
