#include    "srvpi.h"


SrvPI::SrvPI(string dbFilename, int connfd):db(DBFILENAME)
{
	this->connfd = connfd;
	connSockStream.init(connfd);
	sessionCommandPacketCount = 0;
	userID ="0";
}
bool SrvPI::recvOnePacket()
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

	return true;
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
   

    //   	vector<string> testVector; 
	// split("home", "///!", testVector);

	// for (vector<string>::iterator iter=testVector.begin(); iter!=testVector.end(); ++iter)
 //   	{
 //    	std::cout << "test[" << *iter << "]" << '\n';
 //   	}
    
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

void SrvPI::cmdUSER()
{
	printf("USER request\n");

	vector<string> paramVector; 
	split(packet.getSBody(), "\t", paramVector);
	std::map<string, string> selectParamMap = {  {"username", paramVector[0]} };
   	if (db.select("user", selectParamMap))
   	{
   		vector< map<string ,string> > resultMapVector = db.getResult();
   		if (!resultMapVector.empty())
   		{
			packet.sendSTAT_OK(connSockStream, "this username exists");
   		} else {
			packet.sendSTAT_ERR(connSockStream, "no such username");
   		}

   	}else {
		packet.sendSTAT_ERR(connSockStream, "Database select error");
   	}
}

void SrvPI::cmdPASS()
{
	printf("PASS request\n");

	vector<string> paramVector; 
	split(packet.getSBody(), DELIMITER, paramVector);

	// for (vector<string>::iterator iter=paramVector.begin(); iter!=paramVector.end(); ++iter)
 //   	{
 //    	std::cout << "paramVector[" << *iter << "]" << '\n';
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
			packet.print();
			packet.sendSTAT_OK(connSockStream, 	"Welcome to tinyFTP, written by Charles Wenchy <wenchy.zwz@gmail.com>\n" \
											   	+ resultMapVector[0]["USERNAME"] + ", your last working directory is: ~" + userRCWD);
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
	vector<string> paramVector; 
	split(packet.getSBody(), DELIMITER, paramVector);

	string msg_o;
	if (!combineAndValidatePath(GET, paramVector[0], msg_o))
   	{
   		packet.sendSTAT_ERR(connSockStream, msg_o.c_str());
		return;
   	}

	string path = userRootDir + userRCWD + "/" + paramVector[0];
	//std::cout << "cmdGET path[" << path << "]" << '\n';
	SrvDTP srvDTP(this->connSockStream, &(this->packet), this->connfd);
	srvDTP.sendFile(path.c_str());

	packet.sendSTAT_EOT(connSockStream);
}

void SrvPI::cmdRGET()
{
	printf("RGET request\n");
	vector<string> paramVector; 
	split(packet.getSBody(), DELIMITER, paramVector);

	string msg_o;
	if (!combineAndValidatePath(GET, paramVector[0], msg_o))
   	{
   		packet.sendSTAT_ERR(connSockStream, msg_o.c_str());
		return;
   	}

		
	while(recvOnePacket())
	{
		switch(packet.getTagid())
		{
			case TAG_CMD:
			{
				switch(packet.getCmdid())
				{
					case GET:
					{
						break;
					}
					case MKDIR:
					{
						//cmdMKDIR(packet.getSBody().c_str());
						break;
					}
					default:
					{
						Error::msg("unknown tagid: %d", packet.getCmdid());
						break;
					}
				}
				break;
			}
			case TAG_STAT:
			{
				switch(packet.getStatid())
				{
					case STAT_OK:
					{
						break;
					}
					case STAT_ERR:
					{
						break;
					}
					case STAT_EOF:
					{
						// fclose
						break;
					}
					case STAT_EOT:
					{
						// fclose
						break;
					}
					default:
					{
						Error::msg("unknown tagid: %d", packet.getStatid());
						break;
					}
				}
				break;
			}
			case TAG_DATA:
			{
				break;
			}
			default:
			{
				Error::msg("unknown tagid: %d", packet.getTagid());
				break;
			}
		}
	}
}

void SrvPI::cmdPUT()
{
	printf("PUT request\n");
	
	vector<string> paramVector; 
	split(packet.getSBody(), DELIMITER, paramVector);
	string path = userRootDir + userRCWD + "/" + paramVector[0];
	//std::cout << "cmdPUT path[" << path << "]" << '\n';
	string msg_o;
	if (!combineAndValidatePath(PUT, paramVector[0], msg_o))
   	{
   		packet.sendSTAT_CFM(connSockStream, msg_o.c_str());
   		recvOnePacket();
   		if(packet.getTagid() == TAG_STAT && packet.getStatid() == STAT_CFM) {
   			packet.print();
			if (packet.getSBody() == "y")
			{
				std::cout << "packet.getSBody() == y" << '\n';
				SrvDTP srvDTP(this->connSockStream,  &(this->packet), this->connfd);
				srvDTP.recvFile(path.c_str());
				return;
			} else {
				return;
			}
		} else {
			Error::msg("STAT_CFM: unknown tagid %d with statid %d", packet.getTagid(), packet.getStatid());
			return;
		}
   	} else {
		SrvDTP srvDTP(this->connSockStream,  &(this->packet), this->connfd);
		srvDTP.recvFile(path.c_str());
   	}
}
void SrvPI::cmdLS()
{
	printf("LS request\n");
	char buf[MAXLINE];
	vector<string> paramVector; 
	split(packet.getSBody(), DELIMITER, paramVector);
	if (paramVector.size() == 0)
	{
		paramVector.push_back(""); // trick, but not elegent
	}
	string msg_o;
	if (!combineAndValidatePath(LS, paramVector[0], msg_o))
   	{
   		packet.sendSTAT_ERR(connSockStream, msg_o.c_str());
		return;
   	}
	string tmpDir = userRootDir + userRCWD + "/" + paramVector[0];
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
			strcat(buf, "\n");
		}

		if ( (sbody.size() + strlen(buf)) > SLICECAP)
		{
			packet.sendDATA_LIST(connSockStream, 0, 0, sbody.size(), sbody.c_str());
			sbody.clear();
		}
		sbody += buf;
		
	}
	if (!sbody.empty())
	{
		packet.sendDATA_LIST(connSockStream, 0, 0, sbody.size(), sbody.c_str());
	}

	packet.sendSTAT_EOT(connSockStream);

}

void SrvPI::cmdCD()
{
	printf("CD request\n");
	vector<string> paramVector; 
	split(packet.getSBody(), DELIMITER, paramVector);
	string msg_o;
   	if (!combineAndValidatePath(CD, paramVector[0], msg_o))
   	{
   		packet.sendSTAT_ERR(connSockStream, msg_o.c_str());
		return;
   	} else {
		packet.sendSTAT_OK(connSockStream, "CWD: ~" + userRCWD);
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
	// S_ISLINGK(st_mode)
	// S_ISREG(st_mode)       
	// S_ISDIR(st_mode)       
	// S_ISCHR(st_mode) 
	// S_ISBLK(st_mode)
	// S_ISSOCK(st_mode)

	printf("RM request\n");
	vector<string> paramVector; 
	split(packet.getSBody(), DELIMITER, paramVector);
	string msg_o;
   	if (!combineAndValidatePath(RM, paramVector[0], msg_o))
   	{
   		packet.sendSTAT_ERR(connSockStream, msg_o.c_str());
		return;
   	} else {
   		string path = userRootDir + userRCWD + "/" + paramVector[0];
   		char buf[MAXLINE]; 
   		if( remove(path.c_str()) !=0 )
		{
			// send STAT_ERR Response 
			// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
			packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
			return;
		} else {
			// send STAT_OK
			packet.sendSTAT_OK(connSockStream, paramVector[0] + " is removed");
			return;
		}
   	}

	// struct stat statBuf; 
	// string path = userRootDir + userRCWD + "/" + packet.getSBody(); 
 //    stat(path.c_str(), &statBuf);
 //    if (S_ISREG(statBuf.st_mode)){
	// 	char buf[MAXLINE];
	// 	if( remove(path.c_str()) !=0 )
	// 	{
	// 		// send STAT_ERR Response 
	// 		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
	// 		packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
	// 		return;
	// 	} else {
	// 		// send STAT_OK
	// 		packet.sendSTAT_OK(connSockStream, packet.getSBody() + "is removed");
	// 		return;
	// 	}
	// } else if (S_ISDIR(statBuf.st_mode)){
	// 	packet.sendSTAT_ERR(connSockStream, "rm: cannot remove '" + packet.getSBody() + "': Is a directory");
	// 	return;
	// }
    
    
	
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
		packet.sendSTAT_OK(connSockStream, ("~" + userRCWD).c_str());
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
	vector<string> paramVector; 
	split(packet.getSBody(), DELIMITER, paramVector);
	string msg_o;
   	if (!combineAndValidatePath(MKDIR, paramVector[0], msg_o))
   	{
   		packet.sendSTAT_ERR(connSockStream, msg_o.c_str());
		return;
   	} else {
   		string path = userRootDir + userRCWD + "/" + paramVector[0];
   		char buf[MAXLINE]; 
   		if(mkdir(path.c_str(), 0777) == -1){
   			// send STAT_ERR Response
			// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
			msg_o += "system call (mkdir): ";
			msg_o += strerror_r(errno, buf, MAXLINE);
			packet.sendSTAT_ERR(connSockStream, msg_o.c_str());
   		}else {
			// send STAT_OK
			packet.sendSTAT_OK(connSockStream, paramVector[0] + " created");
		}
	}
}

bool SrvPI::combineAndValidatePath(uint16_t cmdid, string userinput, string & msg_o)
{
	string absCWD = userRootDir + userRCWD;

	vector<string> absCWDVector;
	split(absCWD, "/", absCWDVector);
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
   	std::cout << "newAbsDir: " << newAbsDir << '\n';
   	// check path, one user can only work in his own working space
   	if (newAbsDir.substr(0, userRootDir.size()) != userRootDir)
   	{
		//std::cout << "Permission denied: " << newAbsDir << '\n';
		msg_o = "Permission denied: " + newAbsDir;
		return false;
   	} else {  
   		return cmdPathProcess(cmdid, newAbsDir, msg_o);
   	}
}

bool SrvPI::cmdPathProcess(uint16_t cmdid, string newAbsDir, string & msg_o)
{

	switch(cmdid)
	{
		case GET:
		{
			struct stat statBuf;
	   		char buf[MAXLINE];
		    int n = stat(newAbsDir.c_str(), &statBuf);
		    if(!n) // stat call success
			{	
				if (S_ISREG(statBuf.st_mode)){
					return true;
			    } else if (S_ISDIR(statBuf.st_mode)){
					msg_o = "get: cannot download [" + newAbsDir + "]: Is a directory";
					return false;
			    } else {
			    	msg_o = "get: [" + newAbsDir + "] not a regular file or directory";
					return false;
			    }
				
			} else { // stat error
				msg_o = strerror_r(errno, buf, MAXLINE);
				return false;
			}
			break;	
		}
		case PUT:
		{
			if ((access(newAbsDir.c_str(), F_OK)) == 0) {
				// send STAT_ERR Response
				string path = newAbsDir.substr(userRootDir.size(), newAbsDir.size() - userRootDir.size());
				if (path.empty())
				{
					path = "/";
				}
				msg_o = "File [~" + path + "] already exists, overwrite (y/n) ? ";
				return false;
			} else {
				return true;
			}
			break;
		}
		case LS:
		{
			DIR * d= opendir(newAbsDir.c_str());
			char buf[MAXLINE];
			if(!d) //On error
			{	
				//msg_o = strerror_r(errno, buf, MAXLINE);
				msg_o = strerror_r(errno, buf, MAXLINE);
				return false;
				
			} else { // dir already exists
				closedir(d);
				return true;
			}
			break;
		}
		case CD:
		{
			DIR * d= opendir(newAbsDir.c_str());
			char buf[MAXLINE];
			if(!d) //On error
			{	
				msg_o = strerror_r(errno, buf, MAXLINE);
				return false;
				
			} else { // dir already exists
				closedir(d);
			}
	   		// update userRCWD
			this->userRCWD = newAbsDir.substr(userRootDir.size(), newAbsDir.size() - userRootDir.size());
			if (this->userRCWD.empty())
			{
				this->userRCWD = "/";
			}
	   		return true;
		}
		case RM:
		{
			// S_ISLINGK(st_mode)
			// S_ISREG(st_mode)       
			// S_ISDIR(st_mode)       
			// S_ISCHR(st_mode) 
			// S_ISBLK(st_mode)
			// S_ISSOCK(st_mode)
	   		struct stat statBuf;
	   		char buf[MAXLINE];
		    int n = stat(newAbsDir.c_str(), &statBuf);
		    if(!n) // stat call success
			{	
				if (S_ISREG(statBuf.st_mode)){
					return true;
			    } else if (S_ISDIR(statBuf.st_mode)){
					msg_o = "rm: cannot remove '" + newAbsDir + "': Is a directory";
					return false;
			    } else {
			    	msg_o = "rm: '" + newAbsDir + "' not a regular file or directory";
					return false;
			    }
				
			} else { // stat error
				msg_o = strerror_r(errno, buf, MAXLINE);
				return false;
			}
			break;
		}	
		case MKDIR:
		{
			DIR * d= opendir(newAbsDir.c_str());
			//char buf[MAXLINE];
			if(!d) // dir not exist
			{	
				return true;
				
			} else { // dir already exists
				closedir(d);
				msg_o = "already exsits: " + newAbsDir;
				return false;
			}
			break;
		}
		default:
		{
			msg_o = "SrvPI::cmdPathProcess: unknown cmdid";
			return false;
			break;
		}
	}
	return false;
}

SrvPI::~SrvPI()
{

}
void SrvPI::saveUserState()
{
	map<string, string> updateParamMap = {  {"RCWD", userRCWD} };
	db.update("user", userID, updateParamMap);
}