#ifndef _TINYFTP_CONTROLPACKET_H_
#define _TINYFTP_CONTROLPACKET_H_

#include    "common.h"
#include    "error.h"


class ControlPacket
{
public:

	ControlPacket(PacketStoreType pstype = HPACKET);

	void init(uint32_t sesid, uint16_t cmdid, uint16_t bsize, char body[CBODYCAP])
	{ 

		cpack->sesid = sesid;
		cpack->cmdid = cmdid;
		cpack->bsize = bsize;
		memcpy(cpack->body, body, CBODYCAP);  
	};
	void reset(PacketStoreType pstype);
	void zero();

	// network byte order to host byte order 
	void ntohp();
	// host byte order to network byte order 
	void htonp();

	void print();


//private:
	CPacket	*cpack;
	PacketStoreType pstype;

};

#endif /* _TINYFTP_CONTROLPACKET_H_ */