#include "server.h"

// std::map< uint32_t, std::pair<int, int> > gsessionMap;
// static uint32_t gsessionID = 1;
// static int gctrListenfd, gdatListenfd;

// void str_echo(int sockfd )
// {
//     ssize_t     n;
//     ControlPacket controlPacket(NPACKET);
//     SockStream connSockStream(sockfd);

//     while (1)
//     {
//         if ( (n = connSockStream.Readn(controlPacket.cpack, CPACKSIZE)) == 0)
//             Error::ret("str_echo: client terminated prematurely");
//         controlPacket.ntohp();
//         controlPacket.print();
//         //Fputs(cpack, stdout);
//         //fprintf(stderr, "%lu, %d: %s", strlen(recvline), recvline[strlen(recvline)-1], recvline);
//         //connSockStream.Writen(recvline, strlen(recvline));
//     }

// }

void * clientConnect(void * arg)
{
    ThreadArg * ptarg = (ThreadArg *)arg;
    SrvPI srvPI;

    while (1)
    {
        srvPI.run(ptarg->fd);
    }

    return(NULL);
}


int main(int argc, char **argv)
{
    struct sockaddr_in  cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buff[MAXLINE];
    int listenfd, srvConnfd;

    Socket listenSocket(SRV_SOCKET, NULL, CTRPORT);
    listenfd = listenSocket.init();

    std::cout << "Listen socket port: " << CTRPORT << std::endl;

    pthread_t tid;
    ThreadArg threadArg;

    while (1)
    {  
        srvConnfd = listenSocket.tcpAccept(listenfd, (SA *) &cliaddr, &len);
        printf("control conection from %s, port %d\n",
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));
        
        threadArg.fd = srvConnfd;
        Pthread_create(&tid, NULL, &clientConnect, &threadArg);
    }
    return 0;   
}


