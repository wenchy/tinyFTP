#include    "srvpi.h"

// SrvPI::SrvPI(int cliCtrConnfd, int cliDatConnfd):packet(HPACKET)
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
	// packet.reset(NPACKET);
	// if ( (n = connSockStream.Readn(packet.cpack, CPACKSIZE)) == 0)
 //        Error::ret("str_echo: client terminated prematurely");
 //    packet.ntohp();
 //    packet.print();
}
void SrvPI::cmd2pack(uint32_t sesid, uint16_t cmdid, std::vector<string> & cmdVector)
{
	packet.reset(HPACKET);

	uint16_t bsize = 18;
	char body[PBODYCAP] = "Server: echo, ctr packet.";
	//packet.init(sesid, cmdid, bsize, body);
}

void SrvPI::cmd2pack(uint32_t sesid, uint16_t cmdid, uint16_t bsize, char body[PBODYCAP])
{
	packet.reset(HPACKET);
	//packet.init(sesid, cmdid, bsize, body);
}

void SrvPI::cmd2pack(uint32_t sesid, uint16_t cmdid, string str)
{
	packet.reset(HPACKET);
	if(str.size() > 65535)
		Error::msg("body size overflow");
	uint16_t bsize = str.size();
	char body[PBODYCAP];
	std::strcpy(body, str.c_str());
	//packet.init(sesid, cmdid, bsize, body);
}

void SrvPI::infoCmd()
{
	packet.print();
	packet.htonp();
	// SockStream connSockStream(cliCtrConnfd);
 //    connSockStream.Writen(packet.ps,  PACKSIZE); 
}