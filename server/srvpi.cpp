#include    "srvpi.h"

void SrvPI::run(int connfd)
{
	connSockStream.init(connfd);

	packet.reset(NPACKET);
	if ( connSockStream.Readn(packet.ps, PACKSIZE) == 0)
	            Error::quit_pthread("client terminated prematurely");
    packet.ntohp();
    packet.print();
    if (packet.ps->tagid == TAG_CMD)
    {
    	switch(packet.ps->cmdid)
		{
			case GET:
				cmdGET();
				break;
			case PUT:
				cmdPUT();
				break;
			default:
				Error::msg("Sorry! this command function not finished yet.\n");
				break;
		}
    } else {
    	Error::msg("Error: received packet is not a command.\n");
    }
	
}
void SrvPI::cmd2pack(uint32_t sesid, uint16_t cmdid, std::vector<string> & cmdVector)
{
	packet.reset(HPACKET);

	//uint16_t bsize = 18;
	//char body[PBODYCAP] = "Server: echo, ctr packet.";
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
	//uint16_t bsize = str.size();
	//char body[PBODYCAP];
	//std::strcpy(body, str.c_str());
	//packet.init(sesid, cmdid, bsize, body);
}
void SrvPI::cmdGET()
{
	printf("GET request\n");

	packet.ps->body[packet.ps->bsize] = 0;
	srvDTP.init(connSockStream);
	srvDTP.sendFile(packet.ps->body);
}
void SrvPI::cmdPUT()
{
	printf("PUT request\n");

	packet.ps->body[packet.ps->bsize] = 0;
	srvDTP.init(connSockStream);
	srvDTP.recvFile(packet.ps->body);
}
void SrvPI::cmdLS()
{
	printf("LS request\n");
}
void SrvPI::cmdCD()
{
	printf("CD request\n");
}
void SrvPI::cmdDELE()
{
	printf("DELE request\n");
}