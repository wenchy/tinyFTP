#include    "clipi.h"

void CliPI::init(const char *host)
{
	int cliCtrConnfd, cliDatConnfd = -1;

    Socket cliCtrSocket(CLI_SOCKET, host, CTRPORT);
    cliCtrConnfd = cliCtrSocket.init();
    ctrConnStream.init(cliCtrConnfd);

    // ssize_t     n;
    // ControlPacket controlPacket(NPACKET);


    // if ( (n = ctrConnStream.Readn(controlPacket.cpack, CPACKSIZE)) == 0)
    //     Error::ret("Control connect: server terminated prematurely");
    // controlPacket.ntohp();
    // controlPacket.print();
 

    // Socket cliDatSocket(CLI_SOCKET, host, DATPORT);
    // cliDatConnfd = cliDatSocket.init();
    // datConnStream.init(cliDatConnfd);







}
void CliPI::run(uint16_t cmdid, std::vector<string> & cmdVector)
{
	this->cmdid = cmdid;
	this->cmdVector = cmdVector;
	cmd2pack(0, cmdid);
	getCmd();

	int n;
	controlPacket.reset(NPACKET);
	if ( (n = ctrConnStream.Readn(controlPacket.cpack, CPACKSIZE)) == 0)
        Error::ret("str_echo: client terminated prematurely");
    controlPacket.ntohp();
    controlPacket.print();
}
void CliPI::cmd2pack(uint32_t sesid, uint16_t cmdid)
{
	controlPacket.reset(HPACKET);

	uint16_t bsize = 18;
	char body[CBODYCAP] = "Hello, ctr packet.";
	controlPacket.init(sesid, cmdid, bsize, body);
}

// void CliPI::cmd2pack(uint32_t sesid, uint16_t cmdid = 0)
// {
// 	controlPacket.reset(HPACKET);

// 	uint16_t bsize = 18;
// 	char body[CBODYCAP] = "Hello, ctr packet.";
// 	controlPacket.init(sesid, cmdid, bsize, body);
// }

void CliPI::getCmd()
{
	controlPacket.print();
	controlPacket.htonp();
    ctrConnStream.Writen(controlPacket.cpack,  CPACKSIZE); 
}
void CliPI::sessionCmd()
{
	cmd2pack();
	getCmd();

	int n;
	controlPacket.reset(NPACKET);
	if ( (n = ctrConnStream.Readn(controlPacket.cpack, CPACKSIZE)) == 0)
        Error::ret("str_echo: client terminated prematurely");
    controlPacket.ntohp();
    controlPacket.print();
}

void CliPI::infoCmd()
{

}