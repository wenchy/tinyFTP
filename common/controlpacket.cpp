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
	
	hp->sesid = ntohl(cpack->sesid);
	hp->cmdid = ntohs(cpack->cmdid);
	hp->bsize = ntohs(cpack->bsize);
	memcpy(hp->body, cpack->body, CBODYCAP);

	this->pstype = HPACKET;
	cpack = hp;

	free(np);
	
}


void ControlPacket::htonp()
{
	if (pstype == NPACKET)
		Error::msg("already in NETWORK byte order");

	CPacket* hp = cpack;
	CPacket* np = (CPacket*) Malloc(CPACKSIZE);
	
	np->sesid = htonl(cpack->sesid);
	np->cmdid = htons(cpack->cmdid);
	np->bsize = htons(cpack->bsize);
	memcpy(np->body, cpack->body, CBODYCAP);

	this->pstype = NPACKET;
	cpack = np;

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