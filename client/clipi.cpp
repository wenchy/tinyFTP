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
	switch(cmdid)
	{
		case GET:
			cmdGET(cmdid, cmdVector);
			break;
		case PUT:
			cmdPUT(cmdid, cmdVector);
			break;
		case LS:
			cmdLS(cmdid, cmdVector);
			break;
		case CD:
			cmdCD(cmdid, cmdVector);
			break;
		case DELE:
			cmdDELE(cmdid, cmdVector);
			break;	
		default:
			Error::msg("Sorry! this command function not finished yet.\n");
			break;
	}
	
}
void CliPI::cmd2pack(uint32_t sesid, uint16_t cmdid, std::vector<string> & cmdVector)
{
	packet.reset(HPACKET);

	uint32_t nslice = 0;
	uint32_t sindex = 0;
	uint16_t statid = 0;
	uint16_t bsize = 0;
	char body[PBODYCAP];
	if(cmdVector.size() > 1)
	{
		std::strcpy(body,cmdVector[1].c_str());
		bsize = strlen(body); 
	}

	// Error::msg("body: %s\n", body);
	packet.fill(sesid, TAG_CMD, cmdid, statid, nslice, sindex, bsize, body);
	packet.print();
	packet.htonp(); 
}

void CliPI::cmdGET(uint16_t cmdid, std::vector<string> & cmdVector)
{
	if(cmdVector.size() != 2)
	{
		Error::msg("command argument fault");
		return;
	}
	printf("GET request\n");

	char pathname[MAXLINE];
	char buf[MAXLINE];
	std::strcpy(pathname,cmdVector[1].c_str()); 
	FILE *fp;
	if ((access(pathname,F_OK)) == 0) {
		snprintf(buf, MAXLINE, "File [%s] already exists", pathname);
		Error::msg("%s", buf);
		return;
	} else if ( (fp = fopen(pathname, "wb")) == NULL) {
		snprintf(buf, MAXLINE, "%s", strerror(errno));
		Error::msg("%s", buf);
		return;
	} else {
		// command to packet
		cmd2pack(0, cmdid, cmdVector);
	    connSockStream.Writen(packet.ps, PACKSIZE);
	}

    // pathname exist on server? need test
    cliDTP.init(connSockStream);
	cliDTP.recvFile(pathname, fp);
 
}
void CliPI::cmdPUT(uint16_t cmdid, std::vector<string> & cmdVector)
{
	if(cmdVector.size() != 2)
	{
		Error::msg("command argument fault");
		return;
	}
	printf("PUT request\n");

	char pathname[MAXLINE];
	char buf[MAXLINE];
	uint32_t nslice;
	int n;
	std::strcpy(pathname,cmdVector[1].c_str()); 
	FILE *fp;
	if ( (fp = fopen(pathname, "rb")) == NULL)
	{
		snprintf(buf, MAXLINE, "%s", strerror(errno));
		Error::msg("%s", buf);
		return;
	} else if ( (n = getFileNslice(pathname, &nslice)) < 0)  {
		if ( n == -2) {
			Error::msg("Too large file size.", buf);
			return;
		} else {
			Error::msg("File stat error.");
			return;
		}
	} else {
		// command to packet
		cmd2pack(0, cmdid, cmdVector);
	    connSockStream.Writen(packet.ps, PACKSIZE);
	}

	cliDTP.init(connSockStream);
	cliDTP.sendFile(pathname, fp, nslice);
}
void CliPI::cmdLS(uint16_t cmdid, std::vector<string> & cmdVector)
{
	if(cmdVector.size() > 2)
	{
		Error::msg("command argument fault");
		return;
	}
	
	cmd2pack(0, cmdid, cmdVector);
	connSockStream.Writen(packet.ps, PACKSIZE);

	int n;
	while(packet.reset(NPACKET), (n = connSockStream.Readn(packet.ps, PACKSIZE)) > 0 ) 
	{
		packet.ntohp();
		if (packet.ps->tagid == TAG_DATA) {
			packet.ps->body[packet.ps->bsize] = 0;
			printf("%s\n", packet.ps->body);
			
		} else if (packet.ps->tagid == TAG_STAT && packet.ps->statid == STAT_EOT){
			packet.ps->body[packet.ps->bsize] = 0;
			printf("%s\n", packet.ps->body);
			break;
		}
	}
	
 
}
void CliPI::cmdCD(uint16_t cmdid, std::vector<string> & cmdVector)
{
	if(cmdVector.size() > 2)
	{
		Error::msg("command argument fault");
		return;
	}
	
 
}
void CliPI::cmdDELE(uint16_t cmdid, std::vector<string> & cmdVector)
{
	if(cmdVector.size() > 2)
	{
		Error::msg("command argument fault");
		return;
	}
	
 
}

void CliPI::sessionCmd()
{

}

int CliPI::getFileNslice(const char *pathname, uint32_t *pnslice_o)  
{  
 
    unsigned long filesize = 0, n = MAXNSLICE;

    struct stat statbuff;  
    if(stat(pathname, &statbuff) < 0){  
        return -1;  // error
    } else {  
        filesize = statbuff.st_size;  
    }  
    if (filesize % SLICECAP == 0)
	{
		 *pnslice_o = filesize/SLICECAP; 
	} else if ( (n = filesize/SLICECAP + 1) > MAXNSLICE ){
		Error::msg("too large file size: %d\n (MAX: %d)", n, MAXNSLICE);
		return -2; 
	} else {
		 *pnslice_o = filesize/SLICECAP + 1; 
	}
  
    return 1;  
}
