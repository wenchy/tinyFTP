#include    "srvdtp.h"

void SrvDTP::init(SockStream & connSockStream)
{ 
	this->connSockStream = connSockStream;
	packet.init();
}
void SrvDTP::sendFile(const char *pathname)
{
	int n;
	uint32_t nslice =0, sindex = 0;
	FILE* fp;	// Yo!
	char buf[MAXLINE];

	if ( (fp = fopen(pathname, "rb")) == NULL)
	{
		// send STAT_ERR Response
		packet.sendSTAT_ERR(connSockStream, strerror(errno));
		return;
	} else if ( (n = getFileNslice(pathname, &nslice)) < 0)  {
		// send ERR Response
		if ( n == -2) {
			snprintf(buf, MAXLINE, "Too large file size");
		} else {
			snprintf(buf, MAXLINE, "File stat error");
		}
		packet.sendSTAT_ERR(connSockStream, buf);
		return;
	} else {
		// send STAT_OK
		packet.sendSTAT_OK(connSockStream);
	}

	char body[PBODYCAP];
	printf("Send file [%s] now\n", pathname);
	while( (n = fread(body, sizeof(char), PBODYCAP, fp)) >0 )
	{
		packet.sendDATA(connSockStream, 0, nslice, ++sindex, n, body);
	}

	fclose(fp);

	// send EOT
	printf("EOT[%s]\n", pathname);
	packet.sendSTAT_EOT(connSockStream);
}
void SrvDTP::recvFile(const char *pathname)
{
	int n;
	char buf[MAXLINE];
	FILE* fp;	// Yo!
	if ((access(pathname,F_OK)) == 0) {
		// send STAT_ERR Response
		snprintf(buf, MAXLINE, "File [%s] already exists", pathname);
		packet.sendSTAT_ERR(connSockStream, buf);
		return;
	} else if ( (fp = fopen(pathname, "wb")) == NULL) {
		// send STAT_ERR Response
		packet.sendSTAT_ERR(connSockStream, strerror(errno));
		return;
	} else {
		// send STAT_OK
		packet.sendSTAT_OK(connSockStream);
	}
	
	printf("Recv file [%s] now\n", pathname);
	int m;
	while (packet.reset(NPACKET), (n = connSockStream.Readn(packet.ps, PACKSIZE)) > 0)
	{
		packet.ntohp();
		//packet.print();
		if(packet.ps->tagid == TAG_DATA) {
			m = fwrite(packet.ps->body, sizeof(char), packet.ps->bsize, fp);
			if (m != packet.ps->bsize)
			{
				Error::msg("Recieved slice %d/%d: %d vs %d Bytes\n", packet.ps->sindex, packet.ps->nslice, packet.ps->bsize, m);
				return;
			}
			//printf("Recieved packet %d: %d vs %d Bytes\n", packet.ps->sindex, packet.ps->bsize, m);
		} else if(packet.ps->tagid == TAG_STAT && packet.ps->statid == STAT_EOT) {
			fclose(fp);
			packet.ps->body[packet.ps->bsize] = 0;
			printf("\n%s %s\n", packet.ps->body, pathname);
			return;
		} else {
			Error::msg("SrvDTP::recvFile: unknown tagid %d with statid %d", packet.ps->tagid, packet.ps->statid);
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
  	//printf("getFileNslice nslice: %u\n", *pnslice_o);
    return 1;  
}