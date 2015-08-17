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
	CliPI(const char *host);
	//void init(const char *host);
	bool recvOnePacket();
	void run(uint16_t cmdid, std::vector<string> & cmdVector);
	void split(std::string src, std::string token, vector<string>& vect);
	void cmd2pack(uint16_t cmdid, std::vector<string> & cmdVector);
	void userpass2pack(uint16_t cmdid, std::vector<string> & cmdVector);
	
	bool cmdUSER(std::vector<string> & cmdVector);
	bool cmdPASS(std::vector<string> & cmdVector);

	void cmdGET(std::vector<string> & cmdVector);
	void cmdGET(string srvpath, string clipath);
	void cmdRGET(std::vector<string> & cmdVector);
	void cmdPUT(std::vector<string> & cmdVector);
	void cmdLS(std::vector<string> & cmdVector);
	void cmdLLS(std::vector<string> & cmdVector);
	void cmdCD(std::vector<string> & cmdVector);
	void cmdLCD(std::vector<string> & cmdVector);
	void cmdRM(std::vector<string> & cmdVector);
	void cmdLRM(std::vector<string> & cmdVector);
	void cmdPWD(std::vector<string> & cmdVector);
	void cmdLPWD(std::vector<string> & cmdVector);
	void cmdMKDIR(std::vector<string> & cmdVector);
	void cmdLMKDIR(std::vector<string> & cmdVector);
	bool cmdLMKDIR(string path);
	void cmdRMDIR(std::vector<string> & cmdVector);
	void cmdQUIT(std::vector<string> & cmdVector);
	void cmdHELP(std::vector<string> & cmdVector);

private:
	int getFileNslice(const char *pathname, uint32_t *pnslice_o); 
	string toUpper(string &s);
	string toLower(string &s);
	bool confirmYN(const char * prompt);
	void rmdirDFS();
	void removeDir(const char *path_raw, bool removeSelf);

	



private:
	Packet packet;
	SockStream connSockStream;
	string userID; // for simple, userID is equal to session ID
	int connfd;
	// uint16_t cmdid;
	// std::vector<string> cmdVector;
	//CliDTP cliDTP;
	static std::map<string, string> helpMap;

};

#endif /* _TINYFTP_CLIPI_H_ */