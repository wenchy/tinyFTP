#ifndef _TINYFTP_CLIDTP_H_
#define _TINYFTP_CLIDTP_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include    "../common/packet.h"
#include    "../common/socket.h"
#include    "../common/sockstream.h"
#include    "clipi.h"

class CliPI;
// Client Data Transfer Process (CliDTP)
class CliDTP
{
public:
	CliDTP(Packet * ppacket, CliPI * pcliPI);
	void recvOnePacket();
	//void sendFile(const char *pathname, FILE *fp, uint32_t nslice);
	void sendFile(const char *pathname, FILE *fp, uint32_t nslice, uint32_t sindex, uint16_t slicecap = SLICECAP);
	void recvFile(const char *filename, FILE *fp, uint32_t nslice, uint32_t sindex, uint16_t slicecap = SLICECAP);
	//int getFileNslice(const char *pathname, uint32_t *pnslice_o );
	string getFileSizeString(const char *pathname); 
	void removeFile(const char *pathname);
	
private:
	Packet * ppacket;
	CliPI * pcliPI;

};

#endif /* _TINYFTP_CLIDTP_H_ */