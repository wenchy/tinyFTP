#ifndef _TINYFTP_SRVPI_H_
#define _TINYFTP_SRVPI_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include    "../common/packet.h"
#include    "../common/socket.h"
#include    "../common/sockstream.h"
#include    "../common/database.h"
#include    "srvdtp.h"
// Server Protocol Interpreter (SrvPI)
class SrvPI
{
public:
	SrvPI(string dbFilename, int connfd);
	void run();
	void cmd2pack(uint32_t sesid, uint16_t cmdid, std::vector<string> & cmdVector);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, uint16_t bsize, char body[PBODYCAP]);
	void cmd2pack(uint32_t sesid, uint16_t cmdid, string str);
	void split(std::string src, std::string token, vector<string>& vect);  

	void cmdUSER();
	void cmdPASS();

	void cmdGET();
	void cmdPUT();
	void cmdLS();
	void cmdCD();
	void cmdRM();
	void cmdPWD();
	void cmdMKDIR();

	

	~SrvPI();
	
	


private:
	Packet packet;
	int connfd;
	SockStream connSockStream;
	SrvDTP srvDTP;
	Database db;

	string userID; // for simple, userID is equal to session ID
	std::string userRootDir;
	std::string userRCWD; // current working directory relative to userRootDir

	bool combineAndValidatePath(uint16_t cmdid, string userinput, string & msg_o);
	void saveUserState();


};

#endif /* _TINYFTP_SRVPI_H_ */