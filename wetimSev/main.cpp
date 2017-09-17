#include "clientevent.h"

int main (int argc, char **argv)
{
    ClientTcpAccept *imListenPtr = ClientTcpAccept::getInstance();
    sockRotating_c *epoll = sockRotating_c::getSockRotaingPtr(1024);

    epoll->addSock2Rotating(imListenPtr);
    epoll->startRotating();
    return 0;
}
