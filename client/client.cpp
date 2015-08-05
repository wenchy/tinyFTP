#include "../common/common.h"
#include "../common/error.h"
#include "../common/socket.h"
#include "../common/sockstream.h"
#include "../common/ftppacket.h"

void
str_cli(int sockfd)
{
    char    sendline[MAXLINE], recvline[MAXLINE];
    SockStream connSockStream(sockfd);
    while (Fgets(sendline, MAXLINE, stdin) != NULL) {

        connSockStream.Writen(sendline, strlen(sendline));

        if (connSockStream.Readline(recvline, MAXLINE) == 0)
            Error::quit("str_cli: server terminated prematurely");

        Fputs(recvline, stdout);
    }
}

int main(int argc, char **argv)
{
    int connfd;// , n
    //char recvline[MAXLINE + 1];
    if (argc != 2 )
        Error::quit("usage: ./cli <IPaddress>");
    
    Socket cliSocket(CLI_SOCKET, argv[1], PORT);
    connfd = cliSocket.init();

    str_cli(connfd);

    // while ( (n  = cliSocket.tcpRecv(connfd, recvline, MAXLINE, 0)) > 0)
    // {
    //     recvline[n] = 0; // null terminate
    //     if (fputs(recvline, stdout) == EOF)
    //         Error::sys("fputs error");
    // }
    // if (n < 0)
    // Error::sys("read error");

    exit(0);

}
