#ifndef _TINYFTP_SOCKSTREAM_H_
#define _TINYFTP_SOCKSTREAM_H_

#include "common.h"
#include "error.h"


class SockStream
{
public:
    SockStream(){};
    SockStream(int fd){ this->fd = fd; };

    void init(int fd){ this->fd = fd; };

    /* Wrapper for readn */
    ssize_t Readn(void *ptr, size_t nbytes);
   
    /* Wrapper for writen */
    void Writen(void *ptr, size_t nbytes);

    
    /* bytes remained in read buffer. */
    ssize_t readlineBuf(void **vptrptr);
    /* Wrapper for readline. */
    ssize_t Readline(void *ptr, size_t maxlen);

private:
    /* Read "n" bytes from a fd. */
    ssize_t readn(void *vptr, size_t n);

    /* Write "n" bytes to a descriptor. */
    ssize_t writen(const void *vptr, size_t n);

    /* Buffered read for readline. */
    ssize_t bufRead(char *ptr);
    /* read one line to a descriptor. */
    ssize_t readline(void *vptr, size_t maxlen);

    int fd;

    int  read_cnt;
    char *read_ptr;
    char read_buf[MAXLINE];

};

#endif /* _TINYFTP_SOCKSTREAM_H_ */