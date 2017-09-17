#include <QHostAddress>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDebug>
#include <string.h>
#include "tcpconnect.h"
#include "immessage_def.h"
#include "immessagchannel.h"


TcpClient::TcpClient()
{
    sockptr = &mTcpSock;
    //connect(sockptr, SIGNAL(connected()), imPanlePtr, SLOT(serverConnected()));
    //connect(sockptr, SIGNAL(disconnected()), this , SLOT(tcpDisconnect()));
    //connect(sockptr, SIGNAL(readyRead()), this, SLOT(tcpDataRecv()));
}

TcpClient::~TcpClient()
{
    if (sockptr){
        if (sockptr->isOpen())
            close();
    }
}

bool TcpClient::connet2Server(const QString &server, int port)
{
    QHostAddress targeAddr;

    if (sockptr->state() == QAbstractSocket::UnconnectedState){
        if (!targeAddr.setAddress(server)){
            return false;
        }
        sockptr->connectToHost(targeAddr, port);
        return sockptr->waitForConnected(10000);
    }
    return 1;
}

unsigned int TcpClient::sendData(const void *p, unsigned int len)
{
    unsigned int slen = 0, r;

    if (sockptr->isOpen()){
        do {
            if ((r = sockptr->write((const char*)p + slen, len - slen)) > 0){
                slen += r;
            }
        }while (r > 0 && sockptr->waitForBytesWritten());
    }
    return slen;
}

bool TcpClient::recvData(QByteArray &data, QString &peerAddr, int &peerPort)
{
    do {
        if (!sockptr->waitForReadyRead(10000)){
            return 0;
        }
        data += sockptr->readAll();
    }while (sockptr-> bytesAvailable());
    peerAddr = sockptr->peerAddress().toString();
    peerPort = sockptr->peerPort();
    return 1;
}

uint32_t TcpClient::getLocalAddr()
{
    bool b = 0;
    uint32_t a = sockptr->localAddress().toIPv4Address(&b);

    if (!b)
        a = -1;
    return a;
}

uint16_t TcpClient::getLocalPort()
{
    return sockptr->localPort();
}

