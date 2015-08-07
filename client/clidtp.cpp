#include    "clidtp.h"

void CliDTP::init(SockStream & connSockStream)
{ 
	this->connSockStream = connSockStream;
	packet.init();
}
void CliDTP::sendFile(const char *filename)
{

	FILE* fp = Fopen(filename, "rb");	// Yo!
	int n;
	int sindex = 0;
	char body[PBODYCAP];
	Error::msg("Sendfile now %s", filename);
	while( (n = fread(body, sizeof(char), PBODYCAP, fp)) >0 )
	{
		Error::msg("Sendfile now %s", filename);
		packet.reset(HPACKET);
		Error::msg("Sendfile now %s", filename);
		packet.init(0, DATA, n, GET, 0, ++sindex, body);
		Error::msg("Sendfile now %s", filename);
		packet.print();
		packet.htonp();
		connSockStream.Writen(packet.ps, PACKSIZE);

		printf("file_block_length:%d\n",n);
	}
}
void CliDTP::recvFile(const char *filename)
{
	Error::msg("Recieved now %s", filename);
	FILE* fp = Fopen(filename, "wb");	// Yo!
	int n, m;
	while (packet.reset(NPACKET), (n = connSockStream.Readn(packet.ps, PACKSIZE)) > 0)
	{
		packet.ntohp();
		//packet.print();
		if(packet.ps->tagid == DATA) {
			m = fwrite(packet.ps->body, sizeof(char), packet.ps->bsize, fp);
			printf("Recieved packet %d: %d vs %d Bytes\n", packet.ps->sindex, packet.ps->bsize, m);
		} else if(packet.ps->tagid == EOT) {
			break;
		} else {
			Error::msg("CliDTP::recvFile: unknown tagid %d", packet.ps->tagid);
			return;
		}
	}
	fclose(fp);
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