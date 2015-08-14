#include 	"packet.h"

Packet::Packet()
{ 
	this->pstype = HPACKET;
	ps = (PacketStruct*) Malloc(PACKSIZE);
	ps->sesid = 0;  
}
// Packet::Packet(PacketStoreType pstype)
// { 
// 	init(pstype); 
// }
// void Packet::init(PacketStoreType pstype)
// { 
// 	this->pstype = pstype;
// 	ps = (PacketStruct*) Malloc(PACKSIZE); 
// }
void Packet::fill(uint16_t tagid, uint16_t cmdid, uint16_t statid, uint32_t nslice, uint32_t sindex, uint16_t bsize, const char * body)
{ 
	ps->tagid = tagid;

	ps->cmdid = cmdid;

	ps->statid = statid;

	ps->nslice = nslice;
	ps->sindex = sindex;

	ps->bsize = bsize;
	if(body != NULL && bsize != 0)
		memcpy(ps->body, body, bsize);  
}
void Packet::fillStat(uint16_t statid, uint16_t bsize, const char * body)
{ 
	ps->tagid = TAG_STAT;

	ps->cmdid = 0;

	ps->statid = statid;

	ps->nslice = 0;
	ps->sindex = 0;

	ps->bsize = bsize;
	if(body != NULL && bsize != 0)
		memcpy(ps->body, body, bsize);  
}
void Packet::fillCmd(uint16_t cmdid, uint16_t bsize, const char * body)
{ 
	ps->tagid = TAG_CMD;

	ps->cmdid = cmdid;

	ps->statid = 0;

	ps->nslice = 0;
	ps->sindex = 0;

	ps->bsize = bsize;
	if(body != NULL && bsize != 0)
		memcpy(ps->body, body, bsize);  
}

void Packet::fillData(uint32_t nslice, uint32_t sindex, uint16_t bsize, char * body)
{ 
	ps->tagid = TAG_DATA;

	ps->cmdid = 0;

	ps->statid = 0;

	ps->nslice = nslice;
	ps->sindex = sindex;

	ps->bsize = bsize;
	if(body != NULL && bsize != 0)
		memcpy(ps->body, body, bsize);  
}

void Packet::fillData(uint32_t nslice, uint32_t sindex, uint16_t bsize, const char * body)
{ 
	ps->tagid = TAG_DATA;

	ps->cmdid = 0;

	ps->statid = 0;

	ps->nslice = nslice;
	ps->sindex = sindex;

	ps->bsize = bsize;
	if(body != NULL && bsize != 0)
		memcpy(ps->body, body, bsize);  
}

void Packet::setSessionID(uint32_t sesid)
{ 
	ps->sesid = sesid; 
	//printf("setSessionID: %u\n", ps->sesid);
}

void Packet::reset(PacketStoreType pstype)
{
	if (this->pstype == NPACKET){
		if (pstype == HPACKET){
			ps->sesid = ntohl(ps->sesid);
		}
	} else if (this->pstype == HPACKET){
		if (pstype == NPACKET){
			ps->sesid = htonl(ps->sesid);
		}
	}
	this->pstype = pstype;

	//must keep sesid
	ps->tagid = 0;

	ps->cmdid = 0;

	ps->statid = 0;

	ps->nslice = 0;
	ps->sindex = 0;

	memset(ps->body, 0, PBODYCAP);
}
void Packet::zero()
{
	memset(ps, 0, PACKSIZE);
}

void Packet::ntohp()
{
	if (pstype == HPACKET){
		Error::msg("already in HOST byte order\n");
		return;
	}
	ps->sesid = ntohl(ps->sesid);
	ps->tagid = ntohs(ps->tagid);
	
	ps->cmdid = ntohs(ps->cmdid);
	ps->statid = ntohs(ps->statid);

	ps->nslice = ntohl(ps->nslice);
	ps->sindex = ntohl(ps->sindex);

	ps->bsize = ntohs(ps->bsize);

	this->pstype = HPACKET;
	
}


void Packet::htonp()
{
	if (pstype == NPACKET){
		Error::msg("already in NETWORK byte order\n");
		return;
	}

	ps->sesid = htonl(ps->sesid);
	ps->tagid = htons(ps->tagid);
	
	ps->cmdid = htons(ps->cmdid);

	ps->statid = htons(ps->statid);

	ps->nslice = htonl(ps->nslice);
	ps->sindex = htonl(ps->sindex);

	ps->bsize = htons(ps->bsize);

	this->pstype = NPACKET;
	
}

void Packet::print()
{
	if (!DEBUG)
		return;
	
	if (pstype == HPACKET)
	{
		printf("\t\t[HOST Packet: %p]\n", ps);
		
	}
	else if (pstype == NPACKET)
	{
		printf("\t\t[NETWORK Packet: %p]\n", ps);
	}
	else {
		Error::msg("unknown PacketStoreType\n");
		return;
	}

	printf("\t\tsesid = %u\n", ps->sesid);
	printf("\t\ttagid = %d\n", ps->tagid);
	printf("\t\tcmdid = %d\n", ps->cmdid);
	printf("\t\tstatid = %d\n", ps->statid);
	printf("\t\tnslice = %u\n", ps->nslice);
	printf("\t\tsindex = %u\n", ps->sindex);
	printf("\t\tbsize = %d\n", ps->bsize);
	printf("\t\tbody = %s\n",  this->getSBody().c_str());
	
	fflush(stdout);
}
void Packet::sendDATA(SockStream & connSockStream, uint32_t nslice, uint32_t sindex, uint16_t bsize, char *body)
{
	//this->print();
	this->reset(HPACKET);
	this->fillData(nslice, sindex, bsize, body);
	//printf("sendDATA:\n");
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}

void Packet::sendDATA(SockStream & connSockStream, uint32_t nslice, uint32_t sindex, uint16_t bsize, const char *body)
{
	//this->print();
	this->reset(HPACKET);
	this->fillData(nslice, sindex, bsize, body);
	printf("sendDATA:\n");
	this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}

// void Packet::sendDATA(SockStream & connSockStream, uint32_t nslice, uint32_t sindex, uint16_t bsize, string sbody)
// {
// 	//this->print();
// 	this->reset(HPACKET);
// 	this->fillData(nslice, sindex, bsize, body);
// 	printf("sendDATA:\n");
// 	this->print();
// 	this->htonp();
// 	connSockStream.Writen(this->ps, PACKSIZE);
// }
void Packet::sendSTAT_OK(SockStream & connSockStream)
{
	// send OK
	this->reset(HPACKET);
	char buf[MAXLINE];
	snprintf(buf, MAXLINE, "\033[32mOK to transfer\033[0m");
	this->fillStat(STAT_OK, strlen(buf), buf);
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}
void Packet::sendSTAT_OK(SockStream & connSockStream, char *msg)
{
	// send OK
	this->reset(HPACKET);
	char buf[MAXLINE];
	snprintf(buf, MAXLINE, "\033[32m%s\033[0m", msg);
	this->fillStat(STAT_OK, strlen(buf), buf);
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}
void Packet::sendSTAT_OK(SockStream & connSockStream, const char *msg)
{
	// send OK
	this->reset(HPACKET);
	char buf[MAXLINE];
	snprintf(buf, MAXLINE, "\033[32m%s\033[0m", msg);
	this->fillStat(STAT_OK, strlen(buf), buf);
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}
void Packet::sendSTAT_OK(SockStream & connSockStream, string msg)
{
	// send OK
	this->reset(HPACKET);
	char buf[MAXLINE];
	snprintf(buf, MAXLINE, "\033[32m%s\033[0m", msg.c_str());
	this->fillStat(STAT_OK, strlen(buf), buf);
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}
void Packet::sendSTAT_OK(SockStream & connSockStream, uint32_t sesid, string msg)
{
	// send OK
	this->reset(HPACKET);
	char buf[MAXLINE];
	snprintf(buf, MAXLINE, "\033[32m%s\033[0m", msg.c_str());
	this->fillStat(STAT_OK, strlen(buf), buf);
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}
void Packet::sendSTAT_ERR(SockStream & connSockStream)
{
	// send EOT
	this->reset(HPACKET);
	char buf[MAXLINE];
	snprintf(buf, MAXLINE, "\033[31mError occurred\033[0m");
	this->fillStat(STAT_ERR, strlen(buf), buf);
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}
void Packet::sendSTAT_ERR(SockStream & connSockStream, char *msg)
{
	// send EOT
	this->reset(HPACKET);
	char buf[MAXLINE];
	snprintf(buf, MAXLINE, "\033[31m%s\033[0m", msg);
	this->fillStat(STAT_ERR, strlen(buf), buf);
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}
void Packet::sendSTAT_ERR(SockStream & connSockStream, const char *msg)
{
	// send EOT
	this->reset(HPACKET);
	char buf[MAXLINE];
	snprintf(buf, MAXLINE, "\033[31m%s\033[0m", msg);
	this->fillStat(STAT_ERR, strlen(buf), buf);
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}
void Packet::sendSTAT_ERR(SockStream & connSockStream, string msg)
{
	// send EOT
	this->reset(HPACKET);
	char buf[MAXLINE];
	snprintf(buf, MAXLINE, "\033[31m%s\033[0m", msg.c_str());
	this->fillStat(STAT_ERR, strlen(buf), buf);
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
	this->fillStat(STAT_EOT, strlen(buf), buf);
	//this->print();
	this->htonp();
	connSockStream.Writen(this->ps, PACKSIZE);
}
PacketStruct * Packet::getPs()
{ 
	return ps;
}
uint32_t Packet::getSesid()
{ 
	return ps->sesid;
}

uint16_t Packet::getTagid()
{ 
	return ps->tagid;
}
uint16_t Packet::getCmdid()
{ 
	return ps->cmdid;
}

uint16_t Packet::getStatid()
{ 
	return ps->statid;
}

uint32_t Packet::getNslice()
{ 
	return ps->nslice;
}

uint32_t Packet::getSindex()
{ 
	return ps->sindex;
}

uint16_t Packet::getBsize()
{ 
	return ps->bsize;
}
char * Packet::getBody()
{ 
	return ps->body;
}
std::string Packet::getSBody()
{ 
	char buf[PBODYCAP + 1] = {0};
	strncpy(buf, ps->body, ps->bsize);
	return string(buf);
}

Packet::~Packet()
{ 
	free(ps); 
}