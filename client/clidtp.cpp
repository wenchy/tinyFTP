#include    "clidtp.h"

CliDTP::CliDTP(SockStream & connSockStream, Packet * ppacket, int connfd)
{ 
	this->connSockStream = connSockStream;
	this->ppacket = ppacket;
	this->connfd = connfd;
}
// void CliDTP::init(SockStream & connSockStream, Packet & packet)
// { 
// 	this->connSockStream = connSockStream;
// 	this->packet = packet;
// }

void CliDTP::recvOnePacket()
{
	Packet & packet = *(this->ppacket);
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
	//packet.print();
}

void CliDTP::sendFile(const char *pathname, FILE *fp, uint32_t nslice)
{
	Packet & packet = *(this->ppacket);
	int n;
	uint32_t sindex = 0;

	char body[PBODYCAP];
	int oldProgress = 0, newProgress = 0;
	string hfilesize =  getFileSizeString(pathname);
	if(nslice == 0)
	{
		printf("\033[32mEOF: 0 bytes\033[0m\n");
		packet.sendSTAT_EOF(connSockStream);
	}
	while( (n = fread(body, sizeof(char), PBODYCAP, fp)) >0 )
	{
		packet.sendDATA(connSockStream, nslice, ++sindex, n, body);
		newProgress = (sindex*1.0)/nslice*100;
		if (newProgress > oldProgress)
		{
			//printf("\033[2K\r\033[0m");
			fprintf(stderr, "\033[2K\r\033[0mProgress [%s %s]: %3d%%", pathname, hfilesize.c_str(), newProgress);
		}
		oldProgress = newProgress;
	}
	
	// send EOF
	fclose(fp);
	printf("\nEOF [%s]\n", pathname);
	packet.sendSTAT_EOF(connSockStream);
}
void CliDTP::recvFile(const char *pathname, FILE *fp)
{
	Packet & packet = *(this->ppacket);
	string hfilesize;
	// first receive response
	recvOnePacket();
	if (packet.getTagid() == TAG_STAT) {
		if (packet.getStatid() == STAT_OK) {
			cout << packet.getSBody() <<endl;
			hfilesize =  packet.getSBody();
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
					fprintf(stderr, "\033[2K\r\033[0mProgress[%s %s]: %3d%%", pathname, hfilesize.c_str(), newProgress);
				}
				oldProgress = newProgress;
			}
			//printf("Recieved packet %d: %d vs %d Bytes\n", packet.ps->sindex, packet.getBsize(), m);
		} else if(packet.getTagid() == TAG_STAT) {
			if (packet.getStatid() == STAT_EOF)
			{
				fclose(fp);
				std::cout << "\n" << packet.getSBody() << std::endl;
				continue;
			} else if (packet.getStatid() == STAT_EOT){
				std::cout << packet.getSBody() << std::endl;
				return;
			} else {
				Error::msg("SrvDTP::recvFile TAG_STAT: unknown statid %d", packet.getStatid());
				return;
			}
		} else {
			Error::msg("CliDTP::recvFile: unknown tagid %hu with statid %hu", packet.getTagid(), packet.getStatid());
			fclose(fp);
			return;
		}
	}
}

// int CliDTP::getFileNslice(const char *pathname,uint32_t *pnslice_o)  
// {  
 
//     unsigned long filesize = 0, n = MAXNSLICE;

//     struct stat statbuff;  
//     if(stat(pathname, &statbuff) < 0){  
//         return -1;  // error
//     } else {  
//         if (statbuff.st_size == 0)
// 		{
// 			return 0; // file is empty.
// 		} else {
// 			filesize = statbuff.st_size;  
// 		}  
//     }  
//     if (filesize % SLICECAP == 0)
// 	{
// 		 *pnslice_o = filesize/SLICECAP; 
// 	} else if ( (n = filesize/SLICECAP + 1) > MAXNSLICE ){
// 		Error::msg("too large file size: %d\n (MAX: %d)", n, MAXNSLICE);
// 		return -2; 
// 	} else {
// 		 *pnslice_o = filesize/SLICECAP + 1; 
// 	}
  
//     return 1;  
// }


string CliDTP::getFileSizeString(const char *pathname)  
{  
 
    unsigned long filesize = 0;
    unsigned long n = 0;
    string hsize_o;
    char buf[MAXLINE];
    unsigned long kbase = 1024;
    unsigned long mbase = 1024 * 1024;
    unsigned long gbase = 1024 * 1024 * 1024;


    struct stat statbuff;  
    if(stat(pathname, &statbuff) < 0){
    	hsize_o = "error"; 
        return hsize_o;  // error
    } else {  
        if (statbuff.st_size == 0)
		{
			hsize_o = "0B"; // file is empty.
		} else {
			filesize = statbuff.st_size;
			if (filesize / kbase == 0)
			{ 
				snprintf(buf, MAXLINE, "%lu", filesize);
				hsize_o += buf;
				hsize_o +="B";
			} else if ( filesize / mbase == 0 ){
				snprintf(buf, MAXLINE, "%lu", filesize / kbase);
				hsize_o += buf;
				n = (filesize % kbase)* 100 / kbase;
				if (n != 0)
				{
					hsize_o += ".";
					snprintf(buf, MAXLINE, "%2lu", n);
					hsize_o += buf;
				}
				hsize_o +="K";
			} else if ( filesize / gbase == 0 ){
				snprintf(buf, MAXLINE, "%2lu", filesize / mbase);
				hsize_o += buf;
				n = (filesize % mbase)* 100 / mbase;
				if (n != 0)
				{
					hsize_o += ".";
					snprintf(buf, MAXLINE, "%2lu", n);
					hsize_o += buf;
				}
				hsize_o +="M";
			} else {
				snprintf(buf, MAXLINE, "%lu", filesize / gbase);
				hsize_o += buf;
				n = (filesize % gbase) * 100 / gbase ;
				//printf("filesize n: %lu\n", n);
				if (n != 0)
				{
					hsize_o += ".";
					snprintf(buf, MAXLINE, "%2lu", n);
					hsize_o += buf;
				}
				hsize_o +="G";
			}
		}  
    }  
	return hsize_o;
}