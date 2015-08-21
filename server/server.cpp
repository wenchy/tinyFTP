#include    "server.h"
#include    "../common/database.h"
void * clientConnect(void * arg)
{
    ThreadArg * pthreadArg = (ThreadArg *)arg;
    SrvPI srvPI(DBFILENAME, pthreadArg->fd);

    while (1)
    {
        srvPI.run();
    }

    delete pthreadArg;
    return(NULL);
}


int main(int argc, char **argv)
{
    {
        Database db(DBFILENAME);
        db.init();
    }
   

    struct sockaddr_in  cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buff[MAXLINE];
    int listenfd, srvConnfd;

    Socket listenSocket(SRV_SOCKET, NULL, CTRPORT);
    listenfd = listenSocket.init();

    std::cout << "Listen socket port: " << CTRPORT << std::endl;

    pthread_t tid;
    

    while (1)
    {  
        srvConnfd = listenSocket.tcpAccept(listenfd, (SA *) &cliaddr, &len);
        printf("connection from %s, port %d\n",
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));
        ThreadArg * pthreadArg = new ThreadArg;
        pthreadArg->fd = srvConnfd;
        Pthread_create(&tid, NULL, &clientConnect, pthreadArg);
    }
    return 0;   
}


