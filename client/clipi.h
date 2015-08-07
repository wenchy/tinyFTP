#ifndef _TINYFTP_CLIPI_H_
#define _TINYFTP_CLIPI_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include    "../common/packet.h"
#include    "../common/sockstream.h"
#include    "../common/socket.h"
#include    "clidtp.h"
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
	void init(const char *host);
	void run(uint16_t cmdid, std::vector<string> & cmdVector);
	void cmd2pack(uint32_t sesid = 0, uint16_t cmdid = 0);
	void cmdGET();
	void cmdPUT();
	void sessionCmd();
	


private:
	Packet packet;

	SockStream connSockStream;
	uint16_t cmdid;
	std::vector<string> cmdVector;
	CliDTP cliDTP;

};

#endif /* _TINYFTP_CLIPI_H_ */