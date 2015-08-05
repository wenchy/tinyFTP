#include "../common/common.h"
#include "../common/error.h"
#include "../common/socket.h"
#include "../common/sockstream.h"
#include "../common/ftppacket.h"

void
str_echo(int sockfd)
{
    ssize_t     n;
    char    recvline[MAXLINE];
    SockStream connSockStream(sockfd);

    while (1)
    {
        if ( (n = connSockStream.Readline(recvline, MAXLINE)) == 0)
            Error::quit("str_echo: client terminated prematurely");
        Fputs(recvline, stdout);
        connSockStream.Writen(recvline, strlen(recvline));
    }

}

int main(int argc, char **argv)
{
    
    int listenfd, connfd;
    socklen_t len;
    struct sockaddr_in  cliaddr;
    char buff[MAXLINE];
    pid_t childpid;
    

    Socket srvSocket(SRV_SOCKET, NULL, PORT);
    listenfd = srvSocket.init();

    while (1)
    {
        printf("%ld\n", PACKSIZE);
        len = sizeof(cliaddr);
        connfd = srvSocket.tcpAccept(listenfd, (SA *) &cliaddr, &len);
        printf("conection from %s, port %d\n",
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));

        if ( (childpid = fork()) == 0)
        {
            srvSocket.tcpClose(listenfd);
            str_echo(connfd);
            exit(0);
        }
        
        srvSocket.tcpClose(connfd);
    }
}

