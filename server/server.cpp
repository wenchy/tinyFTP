#include "server.h"

std::map< uint32_t, std::pair<int, int> > gsessionMap;
static uint32_t gsessionID = 1;
static int gctrListenfd, gdatListenfd;

void str_echo(int sockfd )
{
    ssize_t     n;
    ControlPacket controlPacket(NPACKET);
    SockStream connSockStream(sockfd);

    while (1)
    {
        if ( (n = connSockStream.Readn(controlPacket.cpack, CPACKSIZE)) == 0)
            Error::ret("str_echo: client terminated prematurely");
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

    ssize_t     n;
    ControlPacket controlPacket(NPACKET);
    SockStream connSockStream(ptarg->fd);

    // transfer session ID
    // SrvPI srvPI;
    // srvPI.cmd2pack(ptarg->sesid, INFO, "session id");
    // srvPI.infoCmd();


    while (1)
    {
        if ( (n = connSockStream.Readn(controlPacket.cpack, CPACKSIZE)) == 0)
            Error::quit_pthread("Control connect: client terminated prematurely");
        controlPacket.ntohp();
        controlPacket.print();
    }

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
    struct sockaddr_in  cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buff[MAXLINE];
    DataPacket dataPacket(NPACKET);
    int n;

    // pthread_t tid;
    ThreadArg threadArg;
   
    std::cout << "Data listen socket port: " << DATPORT << std::endl;
    while (1)
    {  
        threadArg.fd = Socket::tcpAccept(gdatListenfd, (SA *) &cliaddr, &len);
        printf("data conection from %s, port %d\n",
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));

        // add data connfd to sessionMap by the received session id
        SockStream connSockStream(threadArg.fd);
        if ( (n = connSockStream.Readn(dataPacket.dpack, CPACKSIZE)) == 0)
            Error::ret("Data connect: client terminated prematurely");
        dataPacket.ntohp();
        dataPacket.print();

        std::map< uint32_t, std::pair<int, int> >::iterator it = gsessionMap.find(dataPacket.dpack->sesid);
        if (it == gsessionMap.end())
            Error::ret("can not find sesid %u", dataPacket.dpack->sesid);
        
        uint32_t sesid = dataPacket.dpack->sesid;
        gsessionMap[sesid].second = threadArg.fd;
        //Pthread_create(&tid, NULL, &dataConnect, &threadArg);
    }

    return(NULL);
}


int main(int argc, char **argv)
{
    struct sockaddr_in  cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buff[MAXLINE];

    
    // session id to control socket fd and data socket fd
    

    Socket ctrListenSocket(SRV_SOCKET, NULL, CTRPORT);
    gctrListenfd = ctrListenSocket.init();

    Socket datListenSocket(SRV_SOCKET, NULL, DATPORT);
    gdatListenfd = datListenSocket.init();

    std::cout << "Control listen socket port: " << CTRPORT << std::endl;

    pthread_t tid; 
    ThreadArg threadArg;

    Pthread_create(&tid, NULL, &dataDaemon, NULL);

    int srvCtrConnfd;
    while (1)
    {  
        srvCtrConnfd = ctrListenSocket.tcpAccept(gctrListenfd, (SA *) &cliaddr, &len);

        uint32_t curSessionID = gsessionID++;
        gsessionMap.insert( std::pair< uint32_t, std::pair<int, int> >(curSessionID, std::pair<int, int>(srvCtrConnfd, -1)) );
        
        for (std::map< uint32_t, std::pair<int, int> >::iterator it = gsessionMap.begin(); it!=gsessionMap.end(); ++it)
            std::cout << it->first << " => " << std::endl;

        printf("control conection from %s, port %d\n",
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));
        

        threadArg.fd = srvCtrConnfd;
        threadArg.sesid = curSessionID;
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




