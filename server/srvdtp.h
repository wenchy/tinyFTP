#ifndef _TINYFTP_CLIPI_H_
#define _TINYFTP_CLIPI_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include    "../common/packet.h"
#include    "../common/socket.h"
#include    "../common/sockstream.h"

// Server Data Transfer Process (SrvDTP)
class SrvDTP
{
public:
	SrvDTP(SockStream & connSockStream, Packet * ppacket, int connfd);
	//void init(SockStream & connSockStream, Packet & packet);
	void recvOnePacket();
	void sendFile(const char *filename);
	void recvFile(const char *pathname);
	int getFileNslice(const char *pathname, uint32_t *pnslice_o );


private:
	Packet * ppacket;
	SockStream connSockStream;
	int connfd;

};

#endif /* _TINYFTP_CLIPI_H_ */