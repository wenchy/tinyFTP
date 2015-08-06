#include "server.h"

void str_echo(int sockfd)
{
    ssize_t     n;
    ControlPacket controlPacket(NPACKET);
    SockStream connSockStream(sockfd);

    while (1)
    {
        if ( (n = connSockStream.Readn(controlPacket.cpack, CPACKSIZE)) == 0)
            Error::quit("str_echo: client terminated prematurely");
        controlPacket.ntohp();
        controlPacket.print();
        //Fputs(cpack, stdout);
        //fprintf(stderr, "%lu, %d: %s", strlen(recvline), recvline[strlen(recvline)-1], recvline);
        //connSockStream.Writen(recvline, strlen(recvline));
    }

}

void * controlConnect(void * arg)
{
    ThreadArg * ptarg = (ThreadArg *)arg;
    str_echo(ptarg->fd);

    return(NULL);
}

void * dataConnect(void * arg)
{
    ThreadArg * ptarg = (ThreadArg *)arg;
    str_echo(ptarg->fd);

    return(NULL);
}

void * dataDaemon(void * arg)
{
    ThreadArg * ptarg = (ThreadArg *)arg;
    int datListenfd = ptarg->fd;

    struct sockaddr_in  cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buff[MAXLINE];

    pthread_t tid;
    ThreadArg threadArg;
   
    std::cout << "Data listen socket port: " << DATPORT << std::endl;
    while (1)
    {  
        threadArg.fd = Socket::tcpAccept(datListenfd, (SA *) &cliaddr, &len);
        printf("data conection from %s, port %d\n",
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));
        
        Pthread_create(&tid, NULL, &dataConnect, &threadArg);
    }

    return(NULL);
}

int main(int argc, char **argv)
{
    struct sockaddr_in  cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buff[MAXLINE];

    int ctrListenfd, datListenfd;

    Socket ctrListenSocket(SRV_SOCKET, NULL, CTRPORT);
    ctrListenfd = ctrListenSocket.init();

    Socket datListenSocket(SRV_SOCKET, NULL, DATPORT);
    datListenfd = datListenSocket.init();

    std::cout << "Control listen socket port: " << CTRPORT << std::endl;

    pthread_t tid;
    ThreadArg threadArg;
    threadArg.fd =  datListenfd; 
    Pthread_create(&tid, NULL, &dataDaemon, &threadArg);

    
    while (1)
    {  
        threadArg.fd = ctrListenSocket.tcpAccept(ctrListenfd, (SA *) &cliaddr, &len);
        printf("control conection from %s, port %d\n",
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));
        

        Pthread_create(&tid, NULL, &controlConnect, &threadArg);
    }
    return 0;   
}

void init()
{
    // command init
    map<const string, const uint16_t> cmdMap = {    {"USER",    USER},
                                                    {"PASS",    PASS},
                                                    {"GET",     GET},
                                                    {"PUT",     PUT},
                                                    {"MGET",    MGET},
                                                    {"MPUT",    MPUT},
                                                    {"DELE",    DELE},
                                                    {"RGET",    RGET},
                                                    {"RPUT",    RPUT},
                                                    {"CD",      CD},
                                                    {"LS",      LS},
                                                    {"MKD",     MKD},
                                                    {"RMD",     RMD},
                                                    {"BINARY",  BINARY},
                                                    {"ASCII",   ASCII},
                                                    {"QUIT",    QUIT},

                                                    {"INFO",    INFO},
                                                    {"EOT",     EOT}        };

    cout << "cmdMap contains:";
    for (auto it = cmdMap.cbegin(); it != cmdMap.cend(); ++it)
        cout << " [" << (*it).first << ":" << (*it).second << "]" << endl;
      cout << '\n';

}

// int main(int argc, char **argv)
// {
//     struct sockaddr_in  cliaddr;
//     socklen_t len = sizeof(cliaddr);
//     char buff[MAXLINE];
//     pid_t childpid;
   
//     // printf("CPACKSIZE: %ld\n", CPACKSIZE);
//     // printf("DPACKSIZE: %ld\n", DPACKSIZE);

//     if ( (childpid = fork()) < 0 ) {
//         Error::sys("error in fork!(data listen)");
//     } else if (childpid== 0){
//         int datListenfd;
//         int srvDatConnfd;

//         Socket datListenSocket(SRV_SOCKET, NULL, DATPORT);
//         datListenfd = datListenSocket.init();

//         std::cout << "Data listen socket port: " << DATPORT << std::endl;

//         while (1)
//         {  
//             srvDatConnfd = datListenSocket.tcpAccept(datListenfd, (SA *) &cliaddr, &len);
//             printf("data conection from %s, port %d\n",
//                     inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));

//             if ( (childpid = fork()) < 0 ) {
//                 Error::sys("error in fork!(data connection)");
//             } else if ( childpid == 0) {
//                 datListenSocket.tcpClose(datListenfd);
//                 str_echo(srvDatConnfd);
//                 exit(0);
//             } 
//             datListenSocket.tcpClose(srvDatConnfd);
//         }
//     } else {
//         int ctrListenfd;
//         int srvCtrConnfd;

//         Socket ctrListenSocket(SRV_SOCKET, NULL, CTRPORT);
//         ctrListenfd = ctrListenSocket.init();

//         std::cout << "Control listen socket port: " << CTRPORT << std::endl;

//         while (1)
//         {
//             srvCtrConnfd = ctrListenSocket.tcpAccept(ctrListenfd, (SA *) &cliaddr, &len);
//             printf("control conection from %s, port %d\n",
//                     inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));

//             if ( (childpid = fork()) < 0 ) {
//                 Error::sys("error in fork!(data connection)");
//             } else if ( childpid == 0) {
//                 ctrListenSocket.tcpClose(ctrListenfd);
//                 str_echo(srvCtrConnfd);
//                 exit(0);
//             } 
//             ctrListenSocket.tcpClose(srvCtrConnfd);
//         }
//     }

//     return 0;   
// }




