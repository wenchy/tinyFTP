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
	void sendFile(const char *pathname, FILE *fp, uint32_t nslice);
	void recvFile(const char *filename, FILE *fp);
	int getFileNslice(const char *pathname, uint32_t *pnslice_o );
	
private:
	Packet packet;
	SockStream connSockStream;

};

#endif /* _TINYFTP_CLIDTP_H_ */