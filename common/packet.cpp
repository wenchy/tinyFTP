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
void Packet::fill(uint32_t sesid, uint16_t tagid, uint16_t cmdid, uint16_t statid, uint32_t nslice, uint32_t sindex, uint16_t bsize, char body[PBODYCAP])
{ 
	ps->sesid = sesid;
	ps->tagid = tagid;

	ps->cmdid = cmdid;

	ps->statid = statid;

	ps->nslice = nslice;
	ps->sindex = sindex;

	ps->bsize = bsize;
	if(body != NULL && bsize != 0)
		memcpy(ps->body, body, PBODYCAP);  
}
void Packet::fillStat(uint32_t sesid, uint16_t statid, uint16_t bsize, char body[PBODYCAP])
{ 
	ps->sesid = sesid;
	ps->tagid = TAG_STAT;

	ps->cmdid = 0;

	ps->statid = statid;

	ps->nslice = 0;
	ps->sindex = 0;

	ps->bsize = bsize;
	if(body != NULL && bsize != 0)
		memcpy(ps->body, body, PBODYCAP);  
}
void Packet::fillCmd(uint32_t sesid, uint16_t cmdid, uint16_t bsize, char body[PBODYCAP])
{ 
	ps->sesid = sesid;
	ps->tagid = TAG_CMD;

	ps->cmdid = cmdid;

	ps->statid = 0;

	ps->nslice = 0;
	ps->sindex = 0;

	ps->bsize = bsize;
	if(body != NULL && bsize != 0)
		memcpy(ps->body, body, PBODYCAP);  
}
void Packet::fillData(uint32_t sesid, uint32_t nslice, uint32_t sindex, uint16_t bsize, char body[PBODYCAP])
{ 
	ps->sesid = sesid;
	ps->tagid = TAG_DATA;

	ps->cmdid = 0;

	ps->statid = 0;

	ps->nslice = nslice;
	ps->sindex = sindex;

	ps->bsize = bsize;
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
		Error::msg("already in HOST byte order\n");

	PacketStruct* np = ps;
	PacketStruct* hp = (PacketStruct*) Malloc(PACKSIZE);
	
	hp->sesid = ntohl(np->sesid);
	hp->tagid = ntohs(np->tagid);
	
	hp->cmdid = ntohs(np->cmdid);
	hp->statid = ntohs(np->statid);

	hp->nslice = ntohl(np->nslice);
	hp->sindex = ntohl(np->sindex);

	hp->bsize = ntohs(np->bsize);
	memcpy(hp->body, np->body, PBODYCAP);
	
	ps = hp;
	this->pstype = HPACKET;

	free(np);
	
}


void Packet::htonp()
{
	if (pstype == NPACKET)
		Error::msg("already in NETWORK byte order\n");

	PacketStruct* hp = ps;
	PacketStruct* np = (PacketStruct*) Malloc(PACKSIZE);
	
	np->sesid = htonl(hp->sesid);
	np->tagid = htons(hp->tagid);
	
	np->cmdid = htons(hp->cmdid);

	np->statid = htons(hp->statid);

	np->nslice = htonl(hp->nslice);
	np->sindex = htonl(hp->sindex);

	np->bsize = htons(hp->bsize);
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
		printf("\t\t[HOST Packet]\n");
		
	}
	else if (pstype == NPACKET)
	{
		printf("\t\t[NETWORK Packet]\n");
	}
	else
		Error::msg("unknown PacketStoreType\n");

	printf("\t\tsesid = %u\n", ps->sesid);
	printf("\t\ttagid = %d\n", ps->tagid);
	printf("\t\tcmdid = %d\n", ps->cmdid);
	printf("\t\tstatid = %d\n", ps->statid);
	printf("\t\tnslice = %u\n", ps->nslice);
	printf("\t\tsindex = %u\n", ps->sindex);
	printf("\t\tbsize = %d\n", ps->bsize);
	printf("\t\tbody = %s\n",  ps->body);

	
	
	fflush(stdout);
}
void Packet::sendDATA(SockStream & connSockStream, uint32_t sesid, uint32_t nslice, uint32_t sindex, uint16_t bsize, char body[PBODYCAP])
{
	this->reset(HPACKET);
	this->fillData(0, nslice, ++sindex, bsize, body);
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
	//printf("file_block_length:%d\n",n);
}
void Packet::sendSTAT_OK(SockStream & connSockStream)
{
	// send OK
	this->reset(HPACKET);
	char buf[MAXLINE];
	snprintf(buf, MAXLINE, "\033[32mOK to transfer\033[0m");
	this->fillStat(0, STAT_OK, strlen(buf), buf);
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}
void Packet::sendSTAT_ERR(SockStream & connSockStream, char *errmsg)
{
	// send EOT
	this->reset(HPACKET);
	char buf[MAXLINE];
	snprintf(buf, MAXLINE, "\033[31m%s\033[0m", errmsg);
	this->fillStat(0, STAT_ERR, strlen(buf), buf);
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}
void Packet::sendSTAT_EOT(SockStream & connSockStream)
{
	// send EOT
	this->reset(HPACKET);
	char buf[MAXLINE];
	snprintf(buf, MAXLINE, "\033[32mEnd of Tansfer\033[0m");
	this->fillStat(0, STAT_EOT, strlen(buf), buf);
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}

Packet::~Packet()
{ 
	free(ps); 
}