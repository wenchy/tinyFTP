#include 	"sockstream.h"

ssize_t						/* Read "n" bytes from a descriptor. */
SockStream::readn(void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = (char *)vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else
				return(-1);
		} else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
}
/* end readn */

ssize_t
SockStream::Readn(void *ptr, size_t nbytes)
{
	ssize_t		n;

	if ( (n = readn(ptr, nbytes)) < 0)
		Error::sys("readn error");
	return(n);
}


ssize_t						/* Write "n" bytes to a descriptor. */
SockStream::writen(const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = (const char*)vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}
/* end writen */

void
SockStream::Writen(void *ptr, size_t nbytes)
{
	ssize_t		n;
	if ( (n = writen(ptr, nbytes)) < 0 || (size_t)n != nbytes)
		Error::sys("writen error");
}

ssize_t SockStream::bufRead(char *ptr)
{

	if (read_cnt <= 0) {
again:
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return(-1);
		} else if (read_cnt == 0)
			return(0);
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return(1);
}

ssize_t SockStream::readline(void *vptr, size_t maxlen)
{
	ssize_t	rc;
	size_t	n;
	char	c, *ptr;

	ptr = (char *)vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = bufRead(&c)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}
/* end readline */

ssize_t SockStream::readlineBuf(void **vptrptr)
{
	if (read_cnt)
		*vptrptr = read_ptr;
	return(read_cnt);
}


ssize_t SockStream::Readline(void *ptr, size_t maxlen)
{
	ssize_t		n;

	if ( (n = readline(ptr, maxlen)) < 0)
		Error::sys("readline error");
	return(n);
}
