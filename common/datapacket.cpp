#include 	"datapacket.h"

DataPacket::DataPacket(PacketStoreType pstype)
{ 
	this->pstype = pstype;
	dpack = (DPacket*) Malloc(DPACKSIZE); 
}
void DataPacket::zero()
{
	memset(dpack, 0, DPACKSIZE);
}
void DataPacket::reset(PacketStoreType pstype)
{
	zero();
	this->pstype = pstype;
}
void DataPacket::ntohp()
{
	if (pstype == HPACKET)
		Error::msg("already in HOST byte order");
	DPacket* np = dpack;
	DPacket* hp = (DPacket*) Malloc(DPACKSIZE);
	
	hp->sesid = ntohl(np->sesid);
	hp->nslice = ntohs(np->nslice);
	hp->sindex = ntohs(np->sindex);
	hp->bsize = ntohs(np->bsize);
	memcpy(hp->body, np->body, DBODYCAP);

	dpack = hp;
	this->pstype = HPACKET;

	free(np);
}

void DataPacket::htonp()
{
	if (pstype == NPACKET)
		Error::msg("already in NETWORK byte order");

	DPacket* hp = dpack;
	DPacket* np = (DPacket*) Malloc(DPACKSIZE);
	
	np->sesid = ntohl(np->sesid);
	np->nslice = ntohs(hp->nslice);
	np->sindex = ntohs(hp->sindex);
	np->bsize = ntohs(hp->bsize);
	memcpy(np->body, hp->body, DBODYCAP);

	dpack = np;
	this->pstype = NPACKET;

	free(hp);

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

	printf("\t\tsesid = %d\n", dpack->sesid);
	printf("\t\tnslice = %d\n", dpack->nslice);
	printf("\t\tsindex = %d\n", dpack->sindex);
	printf("\t\tbsize = %d\n", dpack->bsize);
	printf("\t\tbody = %s\n", dpack->body);
	
	fflush(stdout);
}