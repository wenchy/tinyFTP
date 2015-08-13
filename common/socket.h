#ifndef _TINYFTP_SOCKET_H_
#define _TINYFTP_SOCKET_H_

#include "common.h"
#include "error.h"

class Socket
{
public:
    Socket( SockType socktype,          /*  CLI_SOCKET or  SRV_SOCKET */
            const char *host,   /* NULL means INADDR_ANY for server */
            short port)
    { 
        this->socktype = socktype;
        this->host = host;
        this->port = port; 
    };
        int     init();
static  int     tcpAccept(int fd, struct sockaddr *sa, socklen_t *salenptr);
        void    tcpBind(int fd, const struct sockaddr *sa, socklen_t salen);
        void    tcpConnect(int fd, const struct sockaddr *sa, socklen_t salen);
        void    tcpListen(int fd, int backlog);
        ssize_t tcpRecv(int fd, void *ptr, size_t nbytes, int flags);
        void    tcpSend(int fd, const void *ptr, size_t nbytes, int flags);
        void    tcpSetsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);
        void    tcpShutdown(int fd, int how);
        int     tcpSocket(int family, int type, int protocol);
static  void    tcpClose(int fd);

private:
    SockType socktype; // client or server socket indicator
    const char *host; // IP-Address or domain name
    short port;
    int sockfd; // listenfd for server, connfd for client

};

#endif /* _TINYFTP_SOCKET_H_ */
