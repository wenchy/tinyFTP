#ifndef _TINYFTP_FTPPACKET_H_
#define _TINYFTP_FTPPACKET_H_

#include    "common.h"
#include    "error.h"


class FTPPacket
{
public:
	FTPPacket(Packet packet, PacketStoreType pst)
	{ 
		this->packet = packet;
		this->pst = pst;  
	};
	void zeroPacket(Packet* p);
	Packet* ntohp(Packet* np);
	Packet* htonp(Packet* hp);
	void print(Packet* p, PacketStoreType pst);


private:
	Packet	packet;
	PacketStoreType pst;

};

#endif /* _TINYFTP_FTPPACKET_H_ */