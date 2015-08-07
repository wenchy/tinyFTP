#ifndef _TINYFTP_SRVPI_H_
#define _TINYFTP_SRVPI_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include    "../common/controlpacket.h"
#include    "../common/datapacket.h"
#include    "../common/sockstream.h"

// Server Protocol Interpreter (SrvPI)
class SrvPI
{
public:
	//SrvPI(int srvCtrConnfd, int srvDatConnfd);
	SrvPI(){};
	void run(uint32_t sesid, uint16_t cmdid);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, std::vector<string> & cmdVector);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, uint16_t bsize, char body[CBODYCAP]);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, string str);
	void infoCmd();
	


private:
	ControlPacket controlPacket;
	int cliCtrConnfd, cliDatConnfd;
	uint32_t sesid;
	uint16_t cmdid;


};

#endif /* _TINYFTP_SRVPI_H_ */