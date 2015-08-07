#ifndef _TINYFTP_SRVPI_H_
#define _TINYFTP_SRVPI_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include    "../common/packet.h"
#include    "../common/sockstream.h"

// Server Protocol Interpreter (SrvPI)
class SrvPI
{
public:
	//SrvPI(int srvCtrConnfd, int srvDatConnfd);
	SrvPI(){};
	void run(int connfd);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, std::vector<string> & cmdVector);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, uint16_t bsize, char body[PBODYCAP]);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, string str);

	void cmdGET();
	void cmdPUT();
	void infoCmd();
	


private:
	Packet packet;
	SockStream connSockStream;


};

#endif /* _TINYFTP_SRVPI_H_ */