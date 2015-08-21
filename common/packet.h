#ifndef _TINYFTP_PACKET_H_
#define _TINYFTP_PACKET_H_

#include    "common.h"
#include    "error.h"
#include    "sockstream.h"
#include    "pi.h"

class Packet
{
public:
	Packet(PI * ppi);
	//Packet(PacketStoreType pstype = HPACKET);
	
	~Packet();

	void fillCmd(uint16_t cmdid, uint16_t bsize, const char * body);
	void fillStat(uint16_t statid, uint16_t bsize, const char * body);
	void fillData(uint16_t dataid, uint32_t nslice, uint32_t sindex, uint16_t bsize,const char * body);

	void setSessionID(uint32_t sesid);

	void reset(PacketStoreType pstype);
	void savePacketState();
	void zero();

	// network byte order to host byte order 
	void ntohp();
	// host byte order to network byte order 
	void htonp();

	void print();
	void pprint();

	void sendCMD(uint16_t cmdid, string sbody);
	void sendCMD_GET(const char *body);
	void sendCMD_GET(string sbody);
	void sendCMD_LMKDIR(const char *body);
	void sendCMD_LMKDIR(string sbody);

	//void sendDATA_FILE(uint32_t nslice, uint32_t sindex, uint16_t bsize, char *body);
	void sendDATA_FILE(uint32_t nslice, uint32_t sindex, uint16_t bsize, const char *body);
	
	void sendDATA_LIST(uint32_t nslice, uint32_t sindex, uint16_t bsize, const char *body);
	void sendDATA_LIST(uint32_t nslice, uint32_t sindex, uint16_t bsize, string body);

	void sendDATA_NAME(uint32_t nslice, uint32_t sindex, uint16_t bsize, const char *body);
	void sendDATA_NAME(uint32_t nslice, uint32_t sindex, uint16_t bsize, string body);

	void sendDATA_TEXT(const char *msg);
	void sendDATA_TEXT(uint16_t bsize, const char *body);
	void sendDATA_TEXT(string msg);

	void sendSTAT(uint16_t statid, string body);
	
	void sendSTAT_OK();
	void sendSTAT_OK(const char *msg);
	void sendSTAT_OK(string msg);

	void sendSTAT_BPR(string body);

	void sendSTAT_MD5(string body);

	void sendSTAT_FAIL(string body);


	void sendSTAT_CFM(const char *msg);
	void sendSTAT_CFM(string msg);

	void sendSTAT_ERR();
	void sendSTAT_ERR(const char *msg);
	void sendSTAT_ERR(string msg);

	void sendSTAT_EOF();
	void sendSTAT_EOF(string msg);

	void sendSTAT_EOT();
	void sendSTAT_EOT(string msg);

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

	PacketStruct * getPrePs();
	uint32_t getPreSesid();
	uint16_t getPreTagid();
	uint16_t getPreCmdid();
	uint16_t getPreStatid();
	uint16_t getPreDataid();
	uint32_t getPreNslice();
	string getPreSNslice();
	uint32_t getPreSindex();
	string getPreSSindex();
	uint16_t getPreBsize();

	PacketStruct *prePs; // previous PacketStruct

private:
	PacketStruct *ps;
	
	PacketStoreType pstype;
	PI * ppi;

	//void init(PacketStoreType pstype = HPACKET);
	void fill(uint16_t tagid, uint16_t cmdid, uint16_t statid, uint16_t dataid, uint32_t nslice, uint32_t sindex, uint16_t bsize, const char * body);
	

};

#endif /* _TINYFTP_PACKET_H_ */