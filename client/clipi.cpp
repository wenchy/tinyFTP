#include    "clipi.h"

void CliPI::init(const char *host)
{
	int connfd;

    Socket cliSocket(CLI_SOCKET, host, CTRPORT);
    connfd = cliSocket.init();
    connSockStream.init(connfd);

}
void CliPI::run(uint16_t cmdid, std::vector<string> & cmdVector)
{
	this->cmdid = cmdid;
	this->cmdVector = cmdVector;
	cmd2pack(0, cmdid);
	getCmd();

	int n;
	packet.reset(NPACKET);
	if ( (n = connSockStream.Readn(packet.ps, PACKSIZE)) == 0)
        Error::ret("str_echo: client terminated prematurely");
    packet.ntohp();
    packet.print();
}
void CliPI::cmd2pack(uint32_t sesid, uint16_t cmdid)
{
	packet.reset(HPACKET);

	uint16_t bsize = 18;
	char body[PBODYCAP] = "Hello, packet.";
	packet.init(sesid, INFO, bsize, cmdid, 0, 0, body); 
}

// void CliPI::cmd2packet(uint32_t sesid, uint16_t cmdid = 0)
// {
// 	packet.reset(HpacketET);

// 	uint16_t bsize = 18;
// 	char body[PBODYCAP] = "Hello, ctr packetet.";
// 	packet.init(sesid, cmdid, bsize, body);
// }

void CliPI::getCmd()
{
	packet.print();
	packet.htonp();
    connSockStream.Writen(packet.ps,  PACKSIZE); 
}
void CliPI::sessionCmd()
{
	cmd2pack();
	getCmd();

	int n;
	packet.reset(NPACKET);
	if ( (n = connSockStream.Readn(packet.ps, PACKSIZE)) == 0)
        Error::ret("str_echo: client terminated prematurely");
    packet.ntohp();
    packet.print();
}

void CliPI::infoCmd()
{

}