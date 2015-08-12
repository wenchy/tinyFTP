#ifndef _TINYFTP_PACKET_H_
#define _TINYFTP_PACKET_H_

#include    "common.h"
#include    "error.h"
#include    "sockstream.h"

class Packet
{
public:
	Packet();
	//Packet(PacketStoreType pstype = HPACKET);
	void init(PacketStoreType pstype = HPACKET);
	~Packet();

	void fill(uint32_t sesid, uint16_t tagid, uint16_t cmdid, uint16_t statid, uint32_t nslice, uint32_t sindex, uint16_t bsize, char body[PBODYCAP]);
	void fillStat(uint32_t sesid, uint16_t statid, uint16_t bsize, char body[PBODYCAP]);
	void fillCmd(uint32_t sesid, uint16_t cmdid, uint16_t bsize, char body[PBODYCAP]);
	void fillData(uint32_t sesid, uint32_t nslice, uint32_t sindex, uint16_t bsize, char body[PBODYCAP]);
	
	void reset(PacketStoreType pstype);
	void zero();

	// network byte order to host byte order 
	void ntohp();
	// host byte order to network byte order 
	void htonp();

	void print();

	void sendDATA(SockStream & connSockStream, uint32_t sesid, uint32_t nslice, uint32_t sindex, uint16_t bsize, char body[PBODYCAP]);

	void sendSTAT_OK(SockStream & connSockStream);
	void sendSTAT_OK(SockStream & connSockStream, char *msg);
	void sendSTAT_OK(SockStream & connSockStream, const char *msg);
	void sendSTAT_OK(SockStream & connSockStream, string msg);
	void sendSTAT_ERR(SockStream & connSockStream);
	void sendSTAT_ERR(SockStream & connSockStream, char *errmsg);
	void sendSTAT_ERR(SockStream & connSockStream, const char *errmsg);
	void sendSTAT_EOT(SockStream & connSockStream);


//private:
	PacketStruct *ps;
	PacketStoreType pstype;

};

#endif /* _TINYFTP_PACKET_H_ */