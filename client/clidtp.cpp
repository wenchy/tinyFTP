#include    "clidtp.h"

void CliDTP::init(SockStream & connSockStream)
{ 
	this->connSockStream = connSockStream;
	packet.init();
}
void CliDTP::sendFile(const char *pathname)
{

	// FILE* fp = Fopen(pathname, "rb");	// Yo!
	// int n;
	// int sindex = 0;
	// char body[PBODYCAP];
	// Error::msg("Sendfile now %s", pathname);
	// while( (n = fread(body, sizeof(char), PBODYCAP, fp)) >0 )
	// {
	// 	Error::msg("Sendfile now %s", pathname);
	// 	packet.reset(HPACKET);
	// 	Error::msg("Sendfile now %s", pathname);
	// 	packet.fill(0, DATA, n, GET, 0, ++sindex, body);
	// 	Error::msg("Sendfile now %s", pathname);
	// 	packet.print();
	// 	packet.htonp();
	// 	connSockStream.Writen(packet.ps, PACKSIZE);

	// 	printf("file_block_length:%d\n",n);
	// }
}
void CliDTP::recvFile(const char *pathname)
{
	
	int n, m;
	// first receive response
	if(packet.reset(NPACKET), (n = connSockStream.Readn(packet.ps, PACKSIZE)) > 0 ) 
	{
		packet.ntohp();
		if (packet.ps->tagid == TAG_INFO) {
			if (packet.ps->statid == STAT_OK) {
				packet.ps->body[packet.ps->bsize] = 0;
				fprintf(stdout, "%s\n", packet.ps->body);
			} else if (packet.ps->statid == STAT_ERR){
				packet.ps->body[packet.ps->bsize] = 0;
				fprintf(stderr, "%s\n", packet.ps->body);
				return;
			} else {
				Error::msg("CliDTP::recvFile: unknown statid %d", packet.ps->statid);
				return;
			}
			
		} else {
			Error::msg("CliDTP::recvFile: unknown tagid %d", packet.ps->tagid);
			return;
		}
	}

	// second transfer file
	fprintf(stdout, "Recieve file now: %s\n", pathname);
	FILE* fp = Fopen(pathname, "wb");	// Yo!
	while (packet.reset(NPACKET), (n = connSockStream.Readn(packet.ps, PACKSIZE)) > 0)
	{
		packet.ntohp();
		//packet.print();
		if(packet.ps->tagid == TAG_DATA) {
			m = fwrite(packet.ps->body, sizeof(char), packet.ps->bsize, fp);
			//printf("Recieved packet %d: %d vs %d Bytes\n", packet.ps->sindex, packet.ps->bsize, m);
		} else if(packet.ps->tagid == TAG_INFO && packet.ps->statid == STAT_EOT) {
			fclose(fp);
			packet.ps->body[packet.ps->bsize] = 0;
			printf("%s\n", packet.ps->body);
			return;
		} else {
			Error::msg("CliDTP::recvFile: unknown tagid %d with statid %d", packet.ps->tagid, packet.ps->statid);
			return;
		}
	}
}