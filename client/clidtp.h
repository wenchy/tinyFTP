#ifndef _TINYFTP_CLIDTP_H_
#define _TINYFTP_CLIDTP_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include    "../common/packet.h"
#include    "../common/socket.h"
#include    "../common/sockstream.h"

// Client Data Transfer Process (CliDTP)
class CliDTP
{
public:
	CliDTP(SockStream & connSockStream, Packet * ppacket, int connfd);
	//void init(SockStream & connSockStream, Packet & packet);
	void recvOnePacket();
	void sendFile(const char *pathname, FILE *fp, uint32_t nslice);
	void recvFile(const char *filename, FILE *fp);
	//int getFileNslice(const char *pathname, uint32_t *pnslice_o );
	string getFileSizeString(const char *pathname); 
	void removeFile(const char *pathname);
	
private:
	Packet * ppacket;
	SockStream connSockStream;
	int connfd;


};

#endif /* _TINYFTP_CLIDTP_H_ */