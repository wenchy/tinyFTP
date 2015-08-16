#ifndef _TINYFTP_CLIPI_H_
#define _TINYFTP_CLIPI_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include    "../common/packet.h"
#include    "../common/socket.h"
#include    "../common/sockstream.h"
#include    "srvpi.h"

class SrvPI;
// Server Data Transfer Process (SrvDTP)
class SrvDTP
{
public:
	SrvDTP(SockStream & connSockStream, Packet * ppacket, int connfd, SrvPI * psrvPI);
	//void init(SockStream & connSockStream, Packet & packet);
	void sendFile(const char *filename);
	void recvFile(const char *pathname);
	int getFileNslice(const char *pathname, uint32_t *pnslice_o );
	string getFileSizeString(const char *pathname);  


private:
	Packet * ppacket;
	SockStream connSockStream;
	int connfd;
	SrvPI * psrvPI;

};

#endif /* _TINYFTP_CLIPI_H_ */