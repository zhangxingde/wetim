#include "clientcpevent.h"
#include "clienudpevent.h"
#include "include_h/stdebug.h"

int main (int argc, char **argv)
{
    ClientTcpAccept *imListenPtr = ClientTcpAccept::getInstance();
    sockRotating_c *epoll = sockRotating_c::getSockRotaingPtr(1024);
    memPool_c  udpMem(1500, 1024);
    ClientUdpImmesgEvent udpListen;

    udpListen.setMemPoolPtr(&udpMem);
    int usock = sockHandle_c::getUdpSockOnPort(0, 12315);
    if (usock <= 0){
        STD_DEBUG("usodk error : %s", strerror(errno));
        return 1;
    }
    STD_DEBUG("usock = %d", usock);
    udpListen.setSockInetAddr(usock, 0);

    epoll->addSock2Rotating(imListenPtr);
    epoll->addSock2Rotating(&udpListen);
    epoll->startRotating();
    return 0;
}
