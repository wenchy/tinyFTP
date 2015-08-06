#include "../common/common.h"
#include "../common/error.h"
#include "../common/socket.h"
#include "../common/sockstream.h"

#include "ui.h"


int main(int argc, char **argv)
{
    
    //char recvline[MAXLINE + 1];
    if (argc != 2 )
        Error::quit("usage: ./client <IPaddress>");

    int cliCtrConnfd, cliDatConnfd = 1;
    
    
    Socket cliCtrSocket(CLI_SOCKET, argv[1], CTRPORT);
    //Socket cliDatSocket(CLI_SOCKET, argv[1], DATPORT);

    cliCtrConnfd = cliCtrSocket.init();
    //cliDatConnfd = cliDatSocket.init();

    UI userInterface(cliCtrConnfd, cliDatConnfd);
    userInterface.run();

    

   

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
