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
	void run(uint32_t sesid, uint16_t cmdid);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, std::vector<string> & cmdVector);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, uint16_t bsize, char body[PBODYCAP]);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, string str);
	void infoCmd();
	


private:
	Packet packet;
	int cliConnfd;
	uint32_t sesid;
	uint16_t cmdid;


};

#endif /* _TINYFTP_SRVPI_H_ */