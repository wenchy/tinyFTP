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
	SrvDTP(Packet * ppacket, SrvPI * psrvPI);
	//void init(SockStream & connSockStream, Packet & packet);
	void insertNewFileMD5SUM(const char * pathname, Database *pdb);
	void sendFile(const char *pathname, uint32_t nslice, uint32_t sindex, uint16_t slicecap = SLICECAP);
	void recvFile(const char *pathname, uint32_t nslice, uint32_t sindex, uint16_t slicecap = SLICECAP);
	int getFileNslice(const char *pathname, uint32_t *pnslice_o );
	string getFileSizeString(const char *pathname);  


private:
	Packet * ppacket;
	SrvPI * psrvPI;

};

#endif /* _TINYFTP_CLIPI_H_ */