#include 	"controlpacket.h"

ControlPacket::ControlPacket(PacketStoreType pstype)
{ 
	this->pstype = pstype;
	cpack = (CPacket*) Malloc(CPACKSIZE); 
}
void ControlPacket::reset(PacketStoreType pstype)
{
	zero();
	this->pstype = pstype;
}
void ControlPacket::zero()
{
	memset(cpack, 0, CPACKSIZE);
}

void ControlPacket::ntohp()
{
	if (pstype == HPACKET)
		Error::msg("already in HOST byte order");

	CPacket* np = cpack;
	CPacket* hp = (CPacket*) Malloc(CPACKSIZE);
	
	hp->sesid = ntohl(np->sesid);
	hp->cmdid = ntohs(np->cmdid);
	hp->bsize = ntohs(np->bsize);
	memcpy(hp->body, np->body, CBODYCAP);
	
	cpack = hp;
	this->pstype = HPACKET;

	free(np);
	
}


void ControlPacket::htonp()
{
	if (pstype == NPACKET)
		Error::msg("already in NETWORK byte order");

	CPacket* hp = cpack;
	CPacket* np = (CPacket*) Malloc(CPACKSIZE);
	
	np->sesid = htonl(hp->sesid);
	np->cmdid = htons(hp->cmdid);
	np->bsize = htons(hp->bsize);
	memcpy(np->body, hp->body, CBODYCAP);

	cpack = np;
	this->pstype = NPACKET;

	free(hp);
	
}

void ControlPacket::print()
{
	if (!DEBUG)
		return;
	
	if (pstype == HPACKET)
	{
		printf("\t\t[HOST Control Packet]\n");
		
	}
	else if (pstype == NPACKET)
	{
		printf("\t\t[NETWORK Control Packet]\n");
	}
	else
		Error::msg("unknown PacketStoreType");

	printf("\t\tsesid = %d\n", cpack->sesid);
	printf("\t\tcmdid = %d\n", cpack->cmdid);
	printf("\t\tbsize = %d\n", cpack->bsize);
	printf("\t\tbody = %s\n",  cpack->body);

	
	
	fflush(stdout);
}