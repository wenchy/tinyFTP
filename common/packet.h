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
	
	~Packet();

	void fillCmd(uint16_t cmdid, uint16_t bsize, const char * body);
	void fillStat(uint16_t statid, uint16_t bsize, const char * body);
	void fillData(uint16_t dataid, uint32_t nslice, uint32_t sindex, uint16_t bsize,const char * body);

	void setSessionID(uint32_t sesid);

	void reset(PacketStoreType pstype);
	void zero();

	// network byte order to host byte order 
	void ntohp();
	// host byte order to network byte order 
	void htonp();

	void print();

	void sendCMD_GET(SockStream & connSockStream, const char *body);
	void sendCMD_GET(SockStream & connSockStream, string sbody);
	void sendCMD_LMKDIR(SockStream & connSockStream, const char *body);
	void sendCMD_LMKDIR(SockStream & connSockStream, string sbody);

	//void sendDATA_FILE(SockStream & connSockStream, uint32_t nslice, uint32_t sindex, uint16_t bsize, char *body);
	void sendDATA_FILE(SockStream & connSockStream, uint32_t nslice, uint32_t sindex, uint16_t bsize, const char *body);
	
	void sendDATA_LIST(SockStream & connSockStream, uint32_t nslice, uint32_t sindex, uint16_t bsize, const char *body);
	void sendDATA_LIST(SockStream & connSockStream, uint32_t nslice, uint32_t sindex, uint16_t bsize, string body);

	void sendDATA_NAME(SockStream & connSockStream, uint32_t nslice, uint32_t sindex, uint16_t bsize, const char *body);
	void sendDATA_NAME(SockStream & connSockStream, uint32_t nslice, uint32_t sindex, uint16_t bsize, string body);

	void sendDATA_TIP(SockStream & connSockStream, const char *msg);
	void sendDATA_TIP(SockStream & connSockStream, string msg);

	void sendSTAT_OK(SockStream & connSockStream);
	//void sendSTAT_OK(SockStream & connSockStream, char *msg);
	void sendSTAT_OK(SockStream & connSockStream, const char *msg);
	void sendSTAT_OK(SockStream & connSockStream, string msg);

	//void sendSTAT_CFM(SockStream & connSockStream, char *msg);
	void sendSTAT_CFM(SockStream & connSockStream, const char *msg);
	void sendSTAT_CFM(SockStream & connSockStream, string msg);

	void sendSTAT_ERR(SockStream & connSockStream);
	//void sendSTAT_ERR(SockStream & connSockStream, char *msg);
	void sendSTAT_ERR(SockStream & connSockStream, const char *msg);
	void sendSTAT_ERR(SockStream & connSockStream, string msg);

	void sendSTAT_EOF(SockStream & connSockStream);
	void sendSTAT_EOF(SockStream & connSockStream, string msg);

	void sendSTAT_EOT(SockStream & connSockStream);
	void sendSTAT_EOT(SockStream & connSockStream, string msg);

	PacketStruct * getPs();
	uint32_t getSesid();
	uint16_t getTagid();
	uint16_t getCmdid();
	uint16_t getStatid();
	uint16_t getDataid();
	uint32_t getNslice();
	uint32_t getSindex();
	uint16_t getBsize();
	char * getBody();
	std::string getSBody();

private:
	PacketStruct *ps;
	PacketStoreType pstype;

	//void init(PacketStoreType pstype = HPACKET);
	void fill(uint16_t tagid, uint16_t cmdid, uint16_t statid, uint16_t dataid, uint32_t nslice, uint32_t sindex, uint16_t bsize, const char * body);
	

};

#endif /* _TINYFTP_PACKET_H_ */