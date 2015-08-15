#include    "srvdtp.h"

SrvDTP::SrvDTP(SockStream & connSockStream, Packet * ppacket, int connfd)
{ 
	this->connSockStream = connSockStream;
	this->ppacket = ppacket;
	this->connfd = connfd;
}
// void SrvDTP::init(SockStream & connSockStream, Packet & packet)
// { 
// 	this->connSockStream = connSockStream;
// 	this->packet = packet;
// }
void SrvDTP::recvOnePacket()
{
	Packet & packet = *(this->ppacket);
	int n;
	packet.reset(NPACKET);
	if ( (n = connSockStream.Readn(packet.getPs(), PACKSIZE)) == 0)
	{
		Socket::tcpClose(connfd);
		Error::quit_pthread("client terminated prematurely, saveUserState ok");
	} else if (n < 0){
		Error::ret("connSockStream.Readn()");
		Error::quit_pthread("socket connection exception");
	}
	packet.ntohp();
	//packet.print();
}
void SrvDTP::sendFile(const char *pathname)
{
	Packet & packet = *(this->ppacket);
	int n;
	uint32_t nslice =0, sindex = 0;
	FILE* fp;	// Yo!
	char buf[MAXLINE];

	if ( (fp = fopen(pathname, "rb")) == NULL)
	{
		// send STAT_ERR Response
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
		return;
	} else if ( (n = getFileNslice(pathname, &nslice)) <= 0)  {
		if ( n == 0) {
			printf("EOT[%s]\n", pathname);
			fclose(fp);
			packet.sendSTAT_EOF(connSockStream, "EOF: 0 bytes");
		} else if ( n == -2) {
			snprintf(buf, MAXLINE, "Too large file size");
			packet.sendSTAT_ERR(connSockStream, buf);
		} else {
			snprintf(buf, MAXLINE, "File stat error");
			packet.sendSTAT_ERR(connSockStream, buf);
		}
		return;
	} else {
		// send STAT_OK
		packet.sendSTAT_OK(connSockStream, getFileSizeString(pathname));
	}

	char body[PBODYCAP];
	printf("Send [%s] now\n", pathname);
	while( (n = fread(body, sizeof(char), PBODYCAP, fp)) >0 )
	{
		packet.sendDATA_FILE(connSockStream, nslice, ++sindex, n, body);
	}

	
	// send EOF
	fclose(fp);
	printf("EOF [%s]\n", pathname);
	packet.sendSTAT_EOF(connSockStream);
}
void SrvDTP::recvFile(const char *pathname)
{
	Packet & packet = *(this->ppacket);
	char buf[MAXLINE];
	FILE* fp;	// Yo!
	if ( (fp = fopen(pathname, "wb")) == NULL) {
		// send STAT_ERR Response
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		// send STAT_OK
		packet.sendSTAT_OK(connSockStream);
	}
	
	printf("Recv file [%s] now\n", pathname);
	int m;

	while (1)
	{
		recvOnePacket();
		if(packet.getTagid() == TAG_DATA) {
			m = fwrite(packet.getBody(), sizeof(char), packet.getBsize(), fp);
			if (m != packet.getBsize())
			{
				Error::msg("Recieved slice %d/%d: %d vs %d Bytes\n", packet.getSindex(), packet.getNslice(), packet.getBsize(), m);
				return;
			}
			//printf("Recieved packet %d: %d vs %d Bytes\n", packet.ps->sindex, packet.ps->bsize, m);
		} else if(packet.getTagid() == TAG_STAT) {
			if (packet.getStatid() == STAT_EOF)
			{
				fclose(fp);
				std::cout << packet.getSBody() << std::endl;
				continue;
			} else if (packet.getStatid() == STAT_EOT){
				std::cout << packet.getSBody() << std::endl;
				return;
			} else {
				Error::msg("SrvDTP::recvFile TAG_STAT: unknown statid %d", packet.getStatid());
				return;
			}
			
		} else {
			Error::msg("SrvDTP::recvFile: unknown tagid %d with statid %d", packet.getTagid(), packet.getStatid());
			return;
		}
	}
}

int SrvDTP::getFileNslice(const char *pathname,uint32_t *pnslice_o)  
{  
 
    unsigned long filesize = 0, n = MAXNSLICE;

    struct stat statbuff;  
    if(stat(pathname, &statbuff) < 0){  
        return -1;  // error
    } else {
	    if (statbuff.st_size == 0)
		{
			return 0; // file is empty.
		} else {
			filesize = statbuff.st_size;  
		} 
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
  	//printf("getFileNslice nslice: %u\n", *pnslice_o);
    return 1;  
}

string SrvDTP::getFileSizeString(const char *pathname)  
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