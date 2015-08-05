#include 	"ftppacket.h"

void FTPPacket::zeroPacket(Packet* p)
{
	memset(p, 0, PACKSIZE);
}

Packet* FTPPacket::ntohp(Packet* np)
{
	Packet* hp = (Packet*) malloc(PACKSIZE);
	memset(hp, 0, PACKSIZE);
	
	hp->sesid = ntohs(np->sesid);
	hp->type = ntohs(np->type);
	hp->cmdid = ntohs(np->cmdid);
	hp->nslice = ntohs(np->nslice);
	hp->sindex = ntohs(np->sindex);
	hp->bsize = ntohs(np->bsize);
	memcpy(hp->body, np->body, BODYCAP);
	
	return hp;	
}

Packet* FTPPacket::htonp(Packet* hp)
{
	Packet* np = (Packet*) malloc(PACKSIZE);
	memset(np, 0, PACKSIZE);
	
	np->sesid = ntohs(hp->sesid);
	np->type = ntohs(hp->type);
	np->cmdid = ntohs(hp->cmdid);
	np->nslice = ntohs(hp->nslice);
	np->sindex = ntohs(hp->sindex);
	np->bsize = ntohs(hp->bsize);
	memcpy(np->body, hp->body, BODYCAP);
	
	return np;
}

void FTPPacket::print(Packet* p, PacketStoreType pst)
{
	if (!DEBUG)
		return;
	
	if (pst == HPACKET)
		printf("\t\tHOST PACKET\n");
	else if (pst == NPACKET)
		printf("\t\tNETWORK PACKET\n");
	else
		Error::msg("unknown PacketStoreType");
	
	printf("\t\tsesid = %d\n", p->sesid);
	printf("\t\ttype = %d\n", p->type);
	printf("\t\tcmdid = %d\n", p->cmdid);
	printf("\t\tnslice = %d\n", p->nslice);
	printf("\t\tsindex = %d\n", p->sindex);
	printf("\t\tbsize = %d\n", p->bsize);
	printf("\t\tbody = %s\n", p->body);
	
	fflush(stdout);
}