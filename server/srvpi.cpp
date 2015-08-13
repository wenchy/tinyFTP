#include    "srvpi.h"


SrvPI::SrvPI(string dbFilename, int connfd):db(DBFILENAME)
{
	this->connfd = connfd;
	connSockStream.init(connfd);
	sessionCommandPacketCount = 0;
	userID ="0";
}
void SrvPI::recvOnePacket()
{
	int n;
	packet.reset(NPACKET);
	if ( (n = connSockStream.Readn(packet.getPs(), PACKSIZE)) == 0)
	{
		this->saveUserState();
		Socket::tcpClose(connfd);
		Error::quit_pthread("client terminated prematurely, saveUserState ok");
	} else if (n < 0){
		Error::ret("connSockStream.Readn()");
		Error::quit_pthread("socket connection exception");
	}
	packet.ntohp();
	packet.print();
}
void SrvPI::run()
{
	std::cout<<  "connfd: " << connfd <<  " [" << userRootDir <<" " << userRCWD << "]" << std::endl;

	recvOnePacket();
	
	sessionCommandPacketCount++;
	// if (std::stoul(userID) != packet.getSesid())
	// {
	// 	Error::quit_pthread("session failed");
	// }
   
    
    if (packet.getTagid() == TAG_CMD)
    {
    	switch(packet.getCmdid())
		{
			case USER:
				cmdUSER();
				break;
			case PASS:
				cmdPASS();
				break;
			case GET:
				cmdGET();
				break;
			case PUT:
				cmdPUT();
				break;
			case LS:
				cmdLS();
				break;
			case CD:
				cmdCD();
				break;
			case RM:
				cmdRM();
				break;
			case PWD:
				cmdPWD();
				break;
			case MKDIR:
				cmdMKDIR();
				break;
			default:
				Error::msg("Server: Sorry! this command function not finished yet.\n");
				break;
		}
    } else {
    	Error::msg("Error: received packet is not a command.\n");
    }
	
}
void SrvPI::cmd2pack(uint32_t sesid, uint16_t cmdid, std::vector<string> & cmdVector)
{
	packet.reset(HPACKET);

	//uint16_t bsize = 18;
	//char body[PBODYCAP] = "Server: echo, ctr packet.";
	//packet.init(sesid, cmdid, bsize, body);
}

void SrvPI::cmd2pack(uint32_t sesid, uint16_t cmdid, uint16_t bsize, char body[PBODYCAP])
{
	packet.reset(HPACKET);
	//packet.init(sesid, cmdid, bsize, body);
}

void SrvPI::cmd2pack(uint32_t sesid, uint16_t cmdid, string str)
{
	packet.reset(HPACKET);
	if(str.size() > 65535)
		Error::msg("body size overflow");
	//uint16_t bsize = str.size();
	//char body[PBODYCAP];
	//std::strcpy(body, str.c_str());
	//packet.init(sesid, cmdid, bsize, body);
}

void SrvPI::cmdUSER()
{
	printf("USER request\n");
}
void SrvPI::split(std::string src, std::string token, vector<string>& vect)   
{    
    int nbegin=0;
    int nend=0;    
    while(nend != -1 && (unsigned int)nbegin < src.length() )    
    {   
        nend = src.find_first_of(token, nbegin);   
        if(nend == -1) {
        	 vect.push_back(src.substr(nbegin, src.length()-nbegin));
        } else {
        	if (nend != nbegin )
        	{
        		vect.push_back(src.substr(nbegin, nend-nbegin));  
        	}
        	 
        }     
        nbegin = nend + 1;   
    }   
}

// static void Split(const char* content, const char* token, vector<std::string>& vect)   
// {   
//     if(content == NULL)   
//            return;   
//     int len = strlen(content);   
//     if(len  <= 0)   
//         return;   
//     char* pBuf =(char*)malloc(len+1);   
//     strcpy(pBuf , content); 
//     // not thread safe  
//     char* str = strtok(pBuf , token);   
//     while(str != NULL)   
//     {   
//         vect.push_back(str);   
//         str = strtok(NULL, token);   
//     }   
//     free(pBuf);   
// }
void SrvPI::cmdPASS()
{
	printf("PASS request\n");

	vector<string> paramVector; 
	split(packet.getSBody(), "\t", paramVector);

	// for (vector<string>::iterator iter=paramVector.begin(); iter!=paramVector.end(); ++iter)
 //   	{
 //    	std::cout << "paramVector[" << *iter << "]" << '\n';
 //   	}

 //   	vector<string> testVector; 
	// split("///!home//!wenchy///!tony///!", "///!", testVector);

	// for (vector<string>::iterator iter=testVector.begin(); iter!=testVector.end(); ++iter)
 //   	{
 //    	std::cout << "test[" << *iter << "]" << '\n';
 //   	}


   	std::map<string, string> selectParamMap = {  {"username", paramVector[0]}, {"password", paramVector[1]} };
   	if (db.select("user", selectParamMap))
   	{
   		vector< map<string ,string> > resultMapVector = db.getResult();
   		if (!resultMapVector.empty())
   		{
   			// init userID, userRootDir, and userRCWD
			userID = resultMapVector[0]["ID"];
			userRootDir = ROOTDIR + resultMapVector[0]["USERNAME"];
			userRCWD = resultMapVector[0]["RCWD"];
			// set session ID: important
			packet.setSessionID(std::stoul(userID));
			packet.sendSTAT_OK(connSockStream, "Welcome! " + resultMapVector[0]["USERNAME"]);
   		} else {
			packet.sendSTAT_ERR(connSockStream, "error: username mismatch password");
   		}
   	} else {
		packet.sendSTAT_ERR(connSockStream, "Database select error");
   	}
}

void SrvPI::cmdGET()
{
	printf("GET request\n");

	SrvDTP srvDTP(this->connSockStream, this->packet, this->connfd);
	srvDTP.sendFile(packet.getSBody().c_str());
}
void SrvPI::cmdPUT()
{
	printf("PUT request\n");
	
	SrvDTP srvDTP(this->connSockStream, this->packet, this->connfd);
	srvDTP.recvFile(packet.getSBody().c_str());
}
void SrvPI::cmdLS()
{
	printf("LS request\n");
	char buf[MAXLINE];

	string msg_o;
	if (!combineAndValidatePath(LS, packet.getSBody(), msg_o))
   	{
   		packet.sendSTAT_ERR(connSockStream, msg_o.c_str());
		return;
   	}

	string tmpDir = userRootDir + userRCWD + "/" + packet.getSBody();
	DIR * dir= opendir(tmpDir.c_str());
	if(!dir)
	{
		// send STAT_ERR Response
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		// send STAT_OK
		packet.sendSTAT_OK(connSockStream);
	}

	struct dirent* e;
	int cnt = 0;
	char body[PBODYCAP];
	string sbody;
	while( (e = readdir(dir)) )
	{	
		if (e->d_type == 4)
		{
			if (!strcmp(e->d_name, "..") || !strcmp(e->d_name, "."))
			{
				continue;
			}
			if (strlen(e->d_name) > 15)
			{
				if (sbody.empty() || sbody.back() == '\n')
				{
					snprintf(buf, MAXLINE, "\033[36m%s\033[0m\n", e->d_name);
				} else {
					snprintf(buf, MAXLINE, "\n\033[36m%s\033[0m\n", e->d_name);
				}
				cnt = 0;
			} else {
				snprintf(buf, MAXLINE, "\033[36m%-10s\033[0m\t", e->d_name);
				cnt++;
			}
		} else /*if(e->d_type == 8) */ {
			if (strlen(e->d_name) > 15)
			{
				if (sbody.empty() || sbody.back() == '\n')
				{
					snprintf(buf, MAXLINE, "%s\n", e->d_name);
				} else {
					snprintf(buf, MAXLINE, "\n%s\n", e->d_name);
				}
				cnt = 0;
			} else {
				snprintf(buf, MAXLINE, "%-10s\t", e->d_name);
				cnt++;
			}
		}

		if ( cnt !=0 && (cnt % 5) == 0)
		{
			snprintf(buf, MAXLINE, "%s\n", buf);
		}

		if ( (sbody.size() + strlen(buf)) > SLICECAP)
		{
			strcpy(body, sbody.c_str());
			packet.sendDATA(connSockStream, 0, 0, strlen(body), body);
			sbody.clear();
		}
		sbody += buf;
		
	}

	strcpy(body, sbody.c_str());
	packet.sendDATA(connSockStream, 0, 0, strlen(body), body);
	
	packet.sendSTAT_EOT(connSockStream);

}

void SrvPI::cmdCD()
{
	printf("CD request\n");

	string msg_o;
   	if (!combineAndValidatePath(CD, packet.getSBody(), msg_o))
   	{
   		packet.sendSTAT_ERR(connSockStream, msg_o.c_str());
		return;
   	} else {
		packet.sendSTAT_OK(connSockStream, "current working directory: " + userRCWD);
		return;
   	}
	// if( (n = chdir(newAbsDir.c_str())) == -1)
	// {
	// 	// send STAT_ERR Response
	// 	// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
	// 	packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
	// 	return;
	// } else {
	// 	// send STAT_OK
	// 	snprintf(buf, MAXLINE, "server: change CWD to [%s]", packet.ps->body);
	// 	packet.sendSTAT_OK(connSockStream, buf);
	// }
	//packet.sendSTAT_EOT(connSockStream);
}

void SrvPI::cmdRM()
{
	printf("RM request\n");

	char buf[MAXLINE];
	if( remove(packet.getSBody().c_str()) !=0 )
	{
		// send STAT_ERR Response 
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		// send STAT_OK
		packet.sendSTAT_OK(connSockStream, packet.getSBody() + "is removed");
	}
}

void SrvPI::cmdPWD()
{
	printf("PWD request\n");

	if (!packet.getSBody().empty())
	{
		if (packet.getSBody() == "-a")
		{
			packet.sendSTAT_OK(connSockStream, (userRootDir + userRCWD).c_str());
		} else {
			packet.sendSTAT_ERR(connSockStream, "command format error");
		}
		
	} else {
		packet.sendSTAT_OK(connSockStream, userRCWD.c_str());
	}
	

	// char buf[MAXLINE];
	// if( !getcwd(buf, MAXLINE))
	// {
	// 	// send STAT_ERR Response
	// 	// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
	// 	packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
	// 	return;
	// } else {
	// 	// send STAT_OK
	// 	packet.sendSTAT_OK(connSockStream, userRCWD.c_str());
	// }
}

void SrvPI::cmdMKDIR()
{
	printf("MKDIR request\n");

	string msg_o;
   	if (!combineAndValidatePath(MKDIR, packet.getSBody(), msg_o))
   	{
   		packet.sendSTAT_ERR(connSockStream, msg_o.c_str());
		return;
   	}

	char buf[MAXLINE];
	string tmpDir = userRootDir + userRCWD + "/" + packet.getSBody();
	DIR * d= opendir(tmpDir.c_str());
	if(d)
	{	
		packet.sendSTAT_ERR(connSockStream, "already exists");
		closedir(d);
	} else if(mkdir(tmpDir.c_str(), 0777) == -1) {
		// send STAT_ERR Response
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		msg_o += "system call (mkdir): ";
		msg_o += strerror_r(errno, buf, MAXLINE);
		packet.sendSTAT_ERR(connSockStream, msg_o.c_str());
		return;
	} else {
		// send STAT_OK
		packet.sendSTAT_OK(connSockStream, packet.getSBody() + "created");
	}
}


bool SrvPI::combineAndValidatePath(uint16_t cmdid, string userinput, string & msg_o)
{
	string absCWD = userRootDir + userRCWD;

	vector<string> absCWDVector; 
	split(absCWD, "/", absCWDVector);
	// for (vector<string>::iterator iter=absCWDVector.begin(); iter!=absCWDVector.end(); ++iter)
 //   	{
 //    	std::cout << "absCWD[" << *iter << "]" << '\n';
 //   	}

	vector<string> userVector; 
	split(userinput, "/", userVector);
	for (vector<string>::iterator iter=userVector.begin(); iter!=userVector.end(); ++iter)
   	{
    	//std::cout << "userinput[" << *iter << "]" << '\n';
    	if (*iter == "..")
    	{
    		absCWDVector.pop_back();
    	} else if (*iter == ".") {
    		continue;
    	} else {
    		absCWDVector.push_back(*iter);
    	}
   	}

   	string newAbsDir;
   	for (vector<string>::iterator iter=absCWDVector.begin(); iter!=absCWDVector.end(); ++iter)
   	{
    	newAbsDir += "/" + *iter;
   	}
   	//std::cout << newAbsDir << '\n';
   	// check path, one user can only work in his own working space
   	if (newAbsDir.substr(0, userRootDir.size()) != userRootDir)
   	{
		//std::cout << "Permission denied: " << newAbsDir << '\n';
		msg_o = "Permission denied: " + newAbsDir;
		return false;
   	} else {

		DIR * d= opendir(newAbsDir.c_str());
		char buf[MAXLINE];
		if(!d) //On error
		{	
			//msg_o = strerror_r(errno, buf, MAXLINE);
			if (cmdid == MKDIR)
			{
				return true;
			} else {
				msg_o = strerror_r(errno, buf, MAXLINE);
				return false;
			}
			
		} else { // dir already exists
			closedir(d);
			if (cmdid == MKDIR)
			{
				msg_o = "already exsits: " + newAbsDir;
				return false;
			}
		}
   		// update userRCWD
   		if (cmdid == CD)
   		{
   			this->userRCWD = newAbsDir.substr(userRootDir.size(), newAbsDir.size() - userRootDir.size());
   			if (this->userRCWD.empty())
   			{
   				this->userRCWD = "/";
   			}
   		}
   		return true;
   	}
}

SrvPI::~SrvPI()
{

}
void SrvPI::saveUserState()
{
	map<string, string> updateParamMap = {  {"RCWD", userRCWD} };
	db.update("user", userID, updateParamMap);
}