#ifndef _TINYFTP_CLIDTP_H_
#define _TINYFTP_CLIDTP_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include    "../common/packet.h"
#include    "../common/sockstream.h"

// Client Data Transfer Process (CliDTP)
class CliDTP
{
public:
	void init(SockStream & connSockStream);
	void sendFile(const char *filename);
	void recvFile(const char *filename);
	
private:
	Packet packet;
	SockStream connSockStream;

};

#endif /* _TINYFTP_CLIDTP_H_ */