#include 	"datapacket.h"

void DataPacket::zero(DPacket* p)
{
	memset(p, 0, DPACKSIZE);
}

DPacket* DataPacket::ntohp(DPacket* np)
{
	DPacket* hp = (DPacket*) malloc(DPACKSIZE);
	memset(hp, 0, DPACKSIZE);
	
	hp->sesid = ntohl(np->sesid);
	hp->nslice = ntohs(np->nslice);
	hp->sindex = ntohs(np->sindex);
	hp->bsize = ntohs(np->bsize);
	memcpy(hp->body, np->body, DBODYCAP);

	this->pstype = HPACKET;
	
	return hp;	
}

DPacket* DataPacket::htonp(DPacket* hp)
{
	DPacket* np = (DPacket*) malloc(DPACKSIZE);
	memset(np, 0, DPACKSIZE);
	
	np->sesid = ntohl(np->sesid);
	np->nslice = ntohs(hp->nslice);
	np->sindex = ntohs(hp->sindex);
	np->bsize = ntohs(hp->bsize);
	memcpy(np->body, hp->body, DBODYCAP);

	this->pstype = NPACKET;
	
	return np;
}

void DataPacket::print()
{
	if (!DEBUG)
		return;
	
	if (pstype == HPACKET)
	{
		printf("\t\t[HOST Data Packet]\n");
	}
	else if (pstype == NPACKET)
	{
		printf("\t\t[NETWORK Data Packet]\n");
	}
	else
		Error::msg("unknown PacketStoreType");

	printf("\t\tsesid = %d\n", dpack.sesid);
	printf("\t\tnslice = %d\n", dpack.nslice);
	printf("\t\tsindex = %d\n", dpack.sindex);
	printf("\t\tbsize = %d\n", dpack.bsize);
	printf("\t\tbody = %s\n", dpack.body);
	
	fflush(stdout);
}