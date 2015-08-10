#ifndef _TINYFTP_SRVPI_H_
#define _TINYFTP_SRVPI_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include    "../common/packet.h"
#include    "../common/sockstream.h"
#include    "srvdtp.h"
// Server Protocol Interpreter (SrvPI)
class SrvPI
{
public:

	void run(int connfd);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, std::vector<string> & cmdVector);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, uint16_t bsize, char body[PBODYCAP]);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, string str);

	void cmdGET();
	void cmdPUT();
	void cmdLS();
	void cmdCD();
	void cmdRM();
	void cmdPWD();
	void cmdMKDIR();
	
	


private:
	Packet packet;
	SockStream connSockStream;
	SrvDTP srvDTP;


};

#endif /* _TINYFTP_SRVPI_H_ */