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
	void cmd2pack(uint32_t sesid, uint16_t cmdid, std::vector<string> & cmdVector);
	void pass2pack(uint32_t sesid, uint16_t cmdid, std::vector<string> & cmdVector);
	
	bool cmdUSER(std::vector<string> & cmdVector);
	bool cmdPASS(std::vector<string> & cmdVector);

	void cmdGET(std::vector<string> & cmdVector);
	void cmdPUT(std::vector<string> & cmdVector);
	void cmdLS(std::vector<string> & cmdVector);
	void cmdCD(std::vector<string> & cmdVector);
	void cmdRM(std::vector<string> & cmdVector);
	void cmdPWD(std::vector<string> & cmdVector);
	void cmdMKDIR(std::vector<string> & cmdVector);

	void sessionCmd();
	int getFileNslice(const char *pathname, uint32_t *pnslice_o);  


private:
	Packet packet;

	SockStream connSockStream;
	// uint16_t cmdid;
	// std::vector<string> cmdVector;
	CliDTP cliDTP;

};

#endif /* _TINYFTP_CLIPI_H_ */