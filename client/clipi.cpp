#include    "clipi.h"

CliPI::CliPI(int cliCtrConnfd, int cliDatConnfd):controlPacket(HPACKET)
{
	this->cliCtrConnfd = cliCtrConnfd;
	this->cliDatConnfd = cliDatConnfd;

}

void CliPI::cmd2pack(uint16_t cmdid, std::vector<string> & cmdVector)
{
	uint32_t sesid = 1;
	uint16_t bsize = 18;

	char body[CBODYCAP] = "Hello, ctr packet.";
	controlPacket.init(sesid, cmdid, bsize, body);
}

void CliPI::getCmd()
{
	controlPacket.print();
	controlPacket.htonp();
	SockStream connSockStream(cliCtrConnfd);
    connSockStream.Writen(controlPacket.cpack,  CPACKSIZE); 
}

void CliPI::infoCmd()
{

}