#ifndef _TINYFTP_PACKET_H_
#define _TINYFTP_PACKET_H_

#include    "common.h"
#include    "error.h"


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


//private:
	PacketStruct *ps;
	PacketStoreType pstype;

};

#endif /* _TINYFTP_PACKET_H_ */