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
		// send Response
		packet.reset(HPACKET);
		snprintf(buf, MAXLINE, "%s", strerror(errno));
		packet.fillInfo(0, STAT_ERR, strlen(buf), buf);
		//packet.print();
		packet.htonp();
		connSockStream.Writen(packet.ps, PACKSIZE);
		return;
	} else if ( (n = getFileNslice(pathname, &nslice)) < 0)  {
		// send Response
		packet.reset(HPACKET);
		if ( n == -2) {
			snprintf(buf, MAXLINE, "too large file size");
		} else {
			snprintf(buf, MAXLINE, "file stat error");
		}
		packet.fillInfo(0, STAT_ERR, strlen(buf), buf);
		//packet.print();
		packet.htonp();
		connSockStream.Writen(packet.ps, PACKSIZE);
		return;
	} else {
		// send Response
		packet.reset(HPACKET);
		snprintf(buf, MAXLINE, "OK start to transfer");
		packet.fillInfo(0, STAT_OK, strlen(buf), buf);
		packet.print();
		packet.htonp();
		connSockStream.Writen(packet.ps, PACKSIZE);
	}

	char body[PBODYCAP];
	printf("Sendfile [%s] now\n", pathname);
	while( (n = fread(body, sizeof(char), PBODYCAP, fp)) >0 )
	{
		packet.reset(HPACKET);
		packet.fillData(0, nslice, ++sindex, n, body);
		//packet.print();
		packet.htonp();
		connSockStream.Writen(packet.ps, PACKSIZE);
		//printf("file_block_length:%d\n",n);
	}

	// send EOT
	packet.reset(HPACKET);
	snprintf(buf, MAXLINE, "End of Tansfer. (%d slices, last size %d)", nslice, n);
	packet.fillInfo(0, STAT_EOT, strlen(buf), buf);
	packet.print();
	packet.htonp();
	connSockStream.Writen(packet.ps, PACKSIZE);
}
void SrvDTP::recvFile(FILE* f)
{
	// int x;
	// int i = 0, j = 0;
	// if((x = recv(sfd, data, size_packet, 0)) <= 0)
	// 	er("recv()", x);
	// j++;
	// hp = ntohp(data);
	// //printpacket(hp, HP);
	// while(hp->type == DATA)
	// {
	// 	i += fwrite(hp->buffer, 1, hp->datalen, f);
	// 	if((x = recv(sfd, data, size_packet, 0)) <= 0)
	// 		er("recv()", x);
	// 	j++;
	// 	hp = ntohp(data);
	// 	//printpacket(hp, HP);
	// }
	// fprintf(stderr, "\t%d data packet(s) received.\n", --j);	// j decremented because the last packet is EOT.
	// fprintf(stderr, "\t%d byte(s) written.\n", i);
	// if(hp->type == EOT)
	// 	return;
	// else
	// {
	// 	fprintf(stderr, "Error occured while downloading remote file.\n");
	// 	exit(2);
	// }
	// fflush(stderr);
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
  
    return 1;  
}