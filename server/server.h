#ifndef _TINYFTP_SERVER_H_
#define _TINYFTP_SERVER_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include    "../common/controlpacket.h"
#include    "../common/sockstream.h"
#include    "../common/socket.h"

typedef struct threadArg
{
    int fd;
    
} ThreadArg;

#endif /* _TINYFTP_SERVER_H_ */