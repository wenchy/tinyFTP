#include 	"packet.h"

Packet::Packet()
{ 
	init(HPACKET);  
}
// Packet::Packet(PacketStoreType pstype)
// { 
// 	init(pstype); 
// }
void Packet::init(PacketStoreType pstype)
{ 
	this->pstype = pstype;
	ps = (PacketStruct*) Malloc(PACKSIZE); 
}
void Packet::fill(uint32_t sesid, uint16_t tagid, uint16_t bsize, uint16_t cmdid, uint16_t nslice, uint16_t sindex, char body[PBODYCAP])
{ 

	ps->sesid = sesid;
	ps->tagid = tagid;
	ps->bsize = bsize;
	ps->cmdid = cmdid;
	ps->nslice = nslice;
	ps->sindex = sindex;
	if(body != NULL && bsize != 0)
		memcpy(ps->body, body, PBODYCAP);  
}
void Packet::reset(PacketStoreType pstype)
{
	zero();
	this->pstype = pstype;
}
void Packet::zero()
{
	memset(ps, 0, PACKSIZE);
}

void Packet::ntohp()
{
	if (pstype == HPACKET)
		Error::msg("already in HOST byte order");

	PacketStruct* np = ps;
	PacketStruct* hp = (PacketStruct*) Malloc(PACKSIZE);
	
	hp->sesid = ntohl(np->sesid);
	hp->tagid = ntohs(np->tagid);
	hp->bsize = ntohs(np->bsize);
	hp->cmdid = ntohs(np->cmdid);
	hp->nslice = ntohs(np->nslice);
	hp->sindex = ntohs(np->sindex);
	memcpy(hp->body, np->body, PBODYCAP);
	
	ps = hp;
	this->pstype = HPACKET;

	free(np);
	
}


void Packet::htonp()
{
	if (pstype == NPACKET)
		Error::msg("already in NETWORK byte order");

	PacketStruct* hp = ps;
	PacketStruct* np = (PacketStruct*) Malloc(PACKSIZE);
	
	np->sesid = htonl(hp->sesid);
	np->tagid = htons(hp->tagid);
	np->bsize = htons(hp->bsize);
	np->cmdid = htons(hp->cmdid);
	np->nslice = htons(hp->nslice);
	np->sindex = htons(hp->sindex);
	memcpy(np->body, hp->body, PBODYCAP);

	ps = np;
	this->pstype = NPACKET;

	free(hp);
	
}

void Packet::print()
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

	printf("\t\tsesid = %d\n", ps->sesid);
	printf("\t\ttagid = %d\n", ps->tagid);
	printf("\t\tbsize = %d\n", ps->bsize);
	printf("\t\tcmdid = %d\n", ps->cmdid);
	printf("\t\tnslice = %d\n", ps->nslice);
	printf("\t\tsindex = %d\n", ps->sindex);
	printf("\t\tbody = %s\n",  ps->body);

	
	
	fflush(stdout);
}

Packet::~Packet()
{ 
	free(ps); 
}