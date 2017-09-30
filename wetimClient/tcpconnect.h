#ifndef QTCPCONNET_H
#define QTCPCONNET_H
#include <QTcpSocket>
#include <QString>
#include <QObject>

class TcpClient
{

public:
    TcpClient();
    ~TcpClient();
    bool connet2Server (const QString &server, int port);
    void close ()
    {
        sockptr->disconnectFromHost();
        if (sockptr->state() != QAbstractSocket::UnconnectedState)
            sockptr->waitForDisconnected();
    }
    unsigned int sendData (const void *p, unsigned int len);
    bool recvData (QByteArray &data, QString &peerAddr, int &peerPort);
    uint32_t getLocalAddr ();
    uint16_t getLocalPort ();
    uint32_t getPeerAddr ();
    int getErrorNo () {return sockptr->error();}
    QString getErrorString() {return sockptr->errorString();}

private:
    QTcpSocket mTcpSock;
    QTcpSocket *sockptr;
    unsigned short port;
};

#endif // QTCPCONNET_H

