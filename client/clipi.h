#ifndef _TINYFTP_CLIPI_H_
#define _TINYFTP_CLIPI_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include    "../common/controlpacket.h"
#include    "../common/sockstream.h"
	// USER = 1,
	// PASS,
	// GET,
	// PUT,
	// MGET,
	// MPUT,
	// DELE,
	// RGET,
	// RPUT,
	// CD,
	// LS,
	// MKD,
	// PWD,
	// RMD,
	// BINARY,
	// ASCII,
	// QUIT,

	// INFO,
	// EOT

// Client Protocol Interpreter (CliPI)
class CliPI
{
public:
	CliPI(int cliCtrConnfd, int cliDatConnfd);
	void cmd2pack(uint16_t cmdid, std::vector<string> & cmdVector);
	void getCmd();
	void infoCmd();
	


private:
	ControlPacket controlPacket;
	int cliCtrConnfd, cliDatConnfd;

};

#endif /* _TINYFTP_CLIPI_H_ */