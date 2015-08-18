#ifndef _TINYFTP_PI_H_
#define _TINYFTP_PI_H_
#include    "common.h"

class PI
{
public:
	virtual bool recvOnePacket() = 0;
	virtual bool sendOnePacket(PacketStruct * ps, size_t nbytes) = 0;
	// virtual bool recvOnePacket(){ printf("virtual bool recvOnePacket()\n"); return true; }
	// virtual bool sendOnePacket(PacketStruct * ps, size_t nbytes){ printf("virtual bool sendOnePacket(Packet * ps, size_t nbytes)\n"); return true; }
};

#endif /* _TINYFTP_PI_H_ */