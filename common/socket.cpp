#include "socket.h"

int Socket::init()
{
	struct sockaddr_in servaddr;
	int optval = 1;
	sockfd = tcpSocket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	// Eliminates "Address already in use" error from bind
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                    (const void *)&optval, sizeof(int)) < 0)
        Error::sys("setsockopt");

	if (socktype == SRV_SOCKET){
	    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	    tcpBind(sockfd, (SA *) &servaddr, sizeof(servaddr));
	    tcpListen(sockfd, LISTENQ);
	} else if (socktype == CLI_SOCKET){ 
	    if (inet_pton(AF_INET, host, &servaddr.sin_addr) < 0)
	        Error::quit("inet_pton error for %s", host);
	    tcpConnect(sockfd, (SA *)&servaddr, sizeof(servaddr));
	} else {
		Error::quit("Unkown SockType");
	}
	/* For client, this sockfd is connfd,
	 * for server, this sockfd is listenfd.	*/
	return sockfd; 
}
int Socket::tcpAccept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int		n;

again:
	if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef	EPROTO
		if (errno == EPROTO || errno == ECONNABORTED)
#else
		if (errno == ECONNABORTED)
#endif
			goto again;
		else
			Error::sys("accept error");
	}
	return(n);
}

void Socket::tcpBind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
		Error::sys("bind error");
}

void Socket::tcpConnect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (connect(fd, sa, salen) < 0)
		Error::sys("connect error");
}

void Socket::tcpListen(int fd, int backlog)
{
	char	*ptr;

		/*4can override 2nd argument with environment variable */
	if ( (ptr = getenv("LISTENQ")) != NULL)
		backlog = atoi(ptr);

	if (listen(fd, backlog) < 0)
		Error::sys("listen error");
}

ssize_t Socket::tcpRecv(int fd, void *ptr, size_t nbytes, int flags)
{
	ssize_t		n;

	if ( (n = recv(fd, ptr, nbytes, flags)) < 0)
		Error::sys("recv error");
	return(n);
}

void Socket::tcpSend(int fd, const void *ptr, size_t nbytes, int flags)
{
	if (send(fd, ptr, nbytes, flags) != (ssize_t)nbytes)
		Error::sys("send error");
}

void Socket::tcpSetsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	if (setsockopt(fd, level, optname, optval, optlen) < 0)
		Error::sys("setsockopt error");
}

void Socket::tcpShutdown(int fd, int how)
{
	if (shutdown(fd, how) < 0)
		Error::sys("shutdown error");
}

int Socket::tcpSocket(int family, int type, int protocol)
{
	int		n;

	if ( (n = socket(family, type, protocol)) < 0)
		Error::sys("socket error");
	return(n);
}

void Socket::tcpClose(int fd)
{
	if (close(fd) == -1)
		Error::sys("close error");
}