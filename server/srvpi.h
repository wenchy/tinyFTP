#ifndef _TINYFTP_SRVPI_H_
#define _TINYFTP_SRVPI_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include    "../common/packet.h"
#include    "../common/socket.h"
#include    "../common/sockstream.h"
#include    "../common/database.h"
#include    "../common/pi.h"
#include    "srvdtp.h"

// Server Protocol Interpreter (SrvPI)
class SrvPI : public PI
{
public:
	SrvPI(string dbFilename, int connfd);
	bool checkBreakpoint();
	bool recvOnePacket();
	bool sendOnePacket(PacketStruct * ps, size_t nbytes);
	void run();
	void split(std::string src, std::string token, vector<string>& vect);  

	void cmdUSER();
	void cmdPASS();
	void cmdUSERADD();
	void cmdUSERDEL();

	void cmdGET();
	void cmdGET(string pathname);
	void RGET_recurse(string srvpath, string clipath);
	void RGET_iterate(string srvpath, string clipath);
	void cmdRGET();
	void cmdPUT();
	bool md5check(string & md5str, string newpath);
	void cmdLS();
	void cmdCD();
	void cmdRM();
	void cmdPWD();
	void cmdMKDIR();
	void cmdRMDIR();
	void cmdSHELL();

	
	int getConnfd();
	FILE* setFp(FILE *fp);
	FILE* & getFp();
	Database * getPDB();
	~SrvPI();
	
	


private:
	int sessionCommandPacketCount;
	Packet packet;
	int connfd;
	SockStream connSockStream;
	//SrvDTP srvDTP;
	Database db;

	string userID; // for simple, userID is equal to session ID
	std::string userRootDir;
	std::string userRCWD; // current working directory relative to userRootDir

	string filename;
	string abspath;
	FILE* fp;
	// -1: error, -2: CFM, 0: ok
	int combineAndValidatePath(uint16_t cmdid, string userinput, string & msg_o, string & abspath_o);
	int cmdPathProcess(uint16_t cmdid, string newAbsDir, string & msg_o);
	void saveUserState();

	void rmdirDFS();
	void removeDir(const char *path_raw, bool removeSelf);


};

#endif /* _TINYFTP_SRVPI_H_ */