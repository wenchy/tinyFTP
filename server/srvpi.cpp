#include    "srvpi.h"

// SrvPI::SrvPI(int cliCtrConnfd, int cliDatConnfd):controlPacket(HPACKET)
// {
// 	this->cliCtrConnfd = cliCtrConnfd;
// 	this->cliDatConnfd = cliDatConnfd;

// }
void SrvPI::run(uint32_t sesid, uint16_t cmdid)
{
	// this->sesid = sesid;
	// this->cmdid = cmdid;
	
	// cmd2pack();
	// getCmd();

	// int n;
	// SockStream connSockStream(cliCtrConnfd);
	// controlPacket.reset(NPACKET);
	// if ( (n = connSockStream.Readn(controlPacket.cpack, CPACKSIZE)) == 0)
 //        Error::ret("str_echo: client terminated prematurely");
 //    controlPacket.ntohp();
 //    controlPacket.print();
}
void SrvPI::cmd2pack(uint32_t sesid, uint16_t cmdid, std::vector<string> & cmdVector)
{
	controlPacket.reset(HPACKET);

	uint16_t bsize = 18;
	char body[CBODYCAP] = "Server: echo, ctr packet.";
	controlPacket.init(sesid, cmdid, bsize, body);
}

void SrvPI::cmd2pack(uint32_t sesid, uint16_t cmdid, uint16_t bsize, char body[CBODYCAP])
{
	controlPacket.reset(HPACKET);
	controlPacket.init(sesid, cmdid, bsize, body);
}

void SrvPI::cmd2pack(uint32_t sesid, uint16_t cmdid, string str)
{
	controlPacket.reset(HPACKET);
	if(str.size() > 65535)
		Error::msg("body size overflow");
	uint16_t bsize = str.size();
	char body[CBODYCAP];
	std::strcpy(body, str.c_str());
	controlPacket.init(sesid, cmdid, bsize, body);
}

void SrvPI::infoCmd()
{
	controlPacket.print();
	controlPacket.htonp();
	SockStream connSockStream(cliCtrConnfd);
    connSockStream.Writen(controlPacket.cpack,  CPACKSIZE); 
}