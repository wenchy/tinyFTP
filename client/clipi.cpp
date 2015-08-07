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

	switch(cmdid)
	{
		case GET:
			cmdGET();
			break;
		case PUT:
			cmdPUT();
			break;
		default:
			Error::msg("unknown command");
			break;
	}
	
}
void CliPI::cmd2pack(uint32_t sesid, uint16_t cmdid)
{
	packet.reset(HPACKET);

	//uint16_t bsize = cmdVector[1].size();

	char body[PBODYCAP];
	std::strcpy(body,cmdVector[1].c_str()); 
	uint16_t bsize = strlen(body);
	Error::msg("cmdVector: %d\n", cmdVector.size());
	// Error::msg("body: %s\n", body);
	packet.fill(sesid, INFO, bsize, cmdid, 0, 0, body);
	packet.print();
	packet.htonp(); 
}

// void CliPI::cmd2packet(uint32_t sesid, uint16_t cmdid = 0)
// {
// 	packet.reset(HpacketET);

// 	uint16_t bsize = 18;
// 	char body[PBODYCAP] = "Hello, ctr packetet.";
// 	packet.init(sesid, cmdid, bsize, body);
// }

void CliPI::cmdGET()
{
	if(cmdVector.size() != 2)
		Error::msg("command argument fault");
	packet.ps->body[packet.ps->bsize] = 0;
	char filename[MAXLINE];
	std::strcpy(filename,cmdVector[1].c_str()); 
	// command to packet
	cmd2pack(0, cmdid);
    connSockStream.Writen(packet.ps,  PACKSIZE);

    // filename is on server? need test
    cliDTP.init(connSockStream);
	cliDTP.recvFile(filename);
	//packet.reset(NPACKET);
 //    int n;
	// while ( (n = connSockStream.Readn(packet.ps, PACKSIZE)) > 0)
	// {
	// 	packet.ntohp();
	// 	Error::msg("Recieved packet %d\n", packet.ps->sindex);
	// 	//int writelen=fwrite(buff,sizeof(char),length,fd);
	// 	packet.reset(NPACKET);
	// }
    

 //    if (packet.ps->tagid == INFO && packet.ps->cmdid == GET && packet.ps->bsize) {
 //    	packet.ps->body[packet.ps->bsize] = 0;
	// 	printf("\t\tGET: %s\n", packet.ps->body);
	// 	//receive_file(chp, data, sfd_client, f);
	// 	//fclose(f);
	// } else {
	// 	Error::msg("Error getting remote file : <%s>\n", packet.ps->body); 
	// }
 
}
void CliPI::cmdPUT()
{
	if(cmdVector.size() != 2)
		Error::msg("command argument fault");
	// command to packet
	cmd2pack(0, cmdid);

	packet.print();
	packet.htonp();
    connSockStream.Writen(packet.ps,  PACKSIZE);
}
void CliPI::sessionCmd()
{

}

