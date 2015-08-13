#include    "clidtp.h"

CliDTP::CliDTP(SockStream & connSockStream, Packet & packet, int connfd)
{ 
	this->connSockStream = connSockStream;
	this->packet = packet;
	this->connfd = connfd;
}
// void CliDTP::init(SockStream & connSockStream, Packet & packet)
// { 
// 	this->connSockStream = connSockStream;
// 	this->packet = packet;
// }

void CliDTP::recvOnePacket()
{
	int n;
	packet.reset(NPACKET);
	if ( (n = connSockStream.Readn(packet.getPs(), PACKSIZE)) == 0)
	{
		Socket::tcpClose(connfd);
		Error::quit("server terminated prematurely");
	} else if (n < 0){
		Error::ret("connSockStream.Readn()");
		Error::quit("socket connection exception");
	}
	packet.ntohp();
	packet.print();
}

void CliDTP::sendFile(const char *pathname, FILE *fp, uint32_t nslice)
{
	int n;
	uint32_t sindex = 0;
	// first PUT response
	recvOnePacket();
	if (packet.getTagid() == TAG_STAT) {
		if (packet.getStatid() == STAT_OK) {
			cout << packet.getSBody() <<endl;
		} else if (packet.getStatid() == STAT_ERR) {
			cerr << packet.getSBody() <<endl;
			return;
		} else {
			
			Error::msg("CliDTP::sendFile: unknown statid %d", packet.getStatid());
			packet.print();
			return;
		}
		
	} else {
		Error::msg("CliDTP::sendFile: unknown tagid %d", packet.getTagid());
		packet.print();
		return;
	}


	char body[PBODYCAP];
	int oldProgress = 0, newProgress = 0;
	if(nslice == 0)
	{
		Error::msg("nslice is zero, can not divide\n");
		return;
	}
	while( (n = fread(body, sizeof(char), PBODYCAP, fp)) >0 )
	{
		packet.sendDATA(connSockStream, nslice, ++sindex, n, body);
		newProgress = (sindex*1.0)/nslice*100;
		if (newProgress > oldProgress)
		{
			//printf("\033[2K\r\033[0m");
			fprintf(stderr, "\033[2K\r\033[0mProgress[%s]: %3d%%", pathname, newProgress);
		}
		oldProgress = newProgress;
	}
	fclose(fp);
	// send EOT
	printf("\nEOT[%s]\n", pathname);
	packet.sendSTAT_EOT(connSockStream);
}
void CliDTP::recvFile(const char *pathname, FILE *fp)
{
	// first receive response
	recvOnePacket();
	if (packet.getTagid() == TAG_STAT) {
		if (packet.getStatid() == STAT_OK) {
			cout << packet.getSBody() <<endl;
		} else if (packet.getStatid() == STAT_ERR){
			cerr << packet.getSBody() <<endl;
			return;
		} else {
			Error::msg("CliDTP::recvFile: unknown statid %d", packet.getStatid());
			return;
		}
		
	} else {
		Error::msg("CliDTP::recvFile: unknown tagid %d", packet.getTagid());
		return;
	}

	// second transfer file
	//fprintf(stdout, "Recieve file now: %s\n", pathname);

	int m;
	int oldProgress = 0, newProgress = 0;
	while (1)
	{
		recvOnePacket();
		if(packet.getTagid() == TAG_DATA) {
			m = fwrite(packet.getBody(), sizeof(char), packet.getBsize(), fp);

			if (m != packet.getBsize())
			{
				Error::msg("fwirte error %d/%d: %d vs %d Bytes\n", packet.getSindex(), packet.getNslice(), packet.getBsize(), m);
				fclose(fp);
				return;
			} else {
				if(packet.getNslice() == 0)
				{
					Error::msg("nslice is zero, can not divide\n");
					break;
				}
				newProgress = (packet.getSindex()*1.0)/packet.getNslice()*100;
				if (newProgress > oldProgress)
				{
					//printf("\033[2K\r\033[0m");
					fprintf(stderr, "\033[2K\r\033[0mProgress[%s]: %3d%%", pathname, newProgress);
				}
				oldProgress = newProgress;
			}
			//printf("Recieved packet %d: %d vs %d Bytes\n", packet.ps->sindex, packet.getBsize(), m);
		} else if(packet.getTagid() == TAG_STAT && packet.getStatid() == STAT_EOT) {
			fclose(fp);
			cout << packet.getSBody() <<endl;
			return;
		} else {
			Error::msg("CliDTP::recvFile: unknown tagid %hu with statid %hu", packet.getTagid(), packet.getStatid());
			fclose(fp);
			return;
		}
	}
}

int CliDTP::getFileNslice(const char *pathname,uint32_t *pnslice_o)  
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