#include    "srvpi.h"


SrvPI::SrvPI(string dbFilename, int connfd): packet(this), db(DBFILENAME)
{
	this->connfd = connfd;
	connSockStream.init(connfd);
	sessionCommandPacketCount = 0;
	userID ="0";
}
void SrvPI::checkBreakpoint()
{

}
bool SrvPI::recvOnePacket()
{
	int n;
	packet.reset(NPACKET);
	if ( (n = connSockStream.readn(packet.getPs(), PACKSIZE)) == 0)
	{
		this->saveUserState();
		Socket::tcpClose(connfd);
		Error::quit_pthread("client terminated prematurely");
	} else if (n < 0){
		this->saveUserState();
		Socket::tcpClose(connfd);
		Error::ret("connSockStream.readn() error");
		Error::quit_pthread("socket connection exception");
	} else {
		packet.ntohp();
		//packet.print();
	}
	return true;
}

bool SrvPI::sendOnePacket(PacketStruct * ps, size_t nbytes)
{
	int n, m;
	bool sendFlag = false;
	int			maxfdp1;
	fd_set		rset, wset;

	FD_ZERO(&rset);
	FD_ZERO(&wset);

	while(!sendFlag) {
		FD_SET(connfd, &rset);
		FD_SET(connfd, &wset);
		maxfdp1 = connfd + 1;
		if (select(maxfdp1, &rset, &wset, NULL, NULL) < 0)
		{	
			this->saveUserState();
			Socket::tcpClose(connfd);
			Error::ret("select error");
			Error::quit_pthread("socket connection exception");
		}

		if (FD_ISSET(connfd, &rset)) /* socket is readable */
		{	
			packet.reset(NPACKET);
			if ( (n = connSockStream.readn(packet.getPs(), PACKSIZE)) == 0)
			{
				this->saveUserState();
				Socket::tcpClose(connfd);
				Error::quit_pthread("client terminated prematurely");
			} else if (n < 0){
				this->saveUserState();
				Socket::tcpClose(connfd);
				Error::ret("connSockStream.readn() error");
				Error::quit_pthread("socket connection exception");
			} else {
				printf("sendOnePacket method recive one packet: %s\n", packet.getSBody().c_str());
				packet.ntohp();
				//packet.print();
			}
		}

		if (FD_ISSET(connfd, &wset)) /* socket is writable */
		{  
			if ( (m = connSockStream.writen(ps, nbytes)) < 0 || (size_t)m != nbytes )
			{
				this->saveUserState();
				Socket::tcpClose(connfd);
				Error::ret("connSockStream.writen()");
				Error::quit_pthread("socket connection exception");
			} else {
				sendFlag = true;
			}
		}
	}
	return true;
}

void SrvPI::run()
{
	recvOnePacket();
	std::cout<<  "\n\n\033[32mNewCMD connfd: " << connfd <<  " [" << userRootDir <<" " << userRCWD << "]\033[0m" << std::endl;
	
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
			case RGET:
				cmdRGET();
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
			case RMDIR:
				cmdRMDIR();
				break;
			case SHELL:
				cmdSHELL();
				break;
			default:
				Error::msg("Server: Sorry! this command function not finished yet.\n");
				break;
		}
    } else {
    	Error::msg("Error: received packet is not a command.\n");
    }
	
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
			packet.sendSTAT_OK("this username exists");
   		} else {
			packet.sendSTAT_ERR("no such username");
   		}

   	}else {
		packet.sendSTAT_ERR("Database select error");
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
			//packet.print();
			packet.sendSTAT_OK("Welcome to tinyFTP, written by Charles Wenchy <wenchy.zwz@gmail.com>\n" \
									 + resultMapVector[0]["USERNAME"] + ", your last working directory is: ~" + userRCWD);
   		} else {
			packet.sendSTAT_ERR("error: username mismatch password");
   		}
   	} else {
		packet.sendSTAT_ERR("Database select error");
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
   		packet.sendSTAT_ERR(msg_o.c_str());
		return;
   	}

	string path = userRootDir + (userRCWD == "/" ? "/": userRCWD + "/") + paramVector[0];
	//std::cout << "cmdGET path[" << path << "]" << '\n';
	SrvDTP srvDTP(&(this->packet), this);
	srvDTP.sendFile(path.c_str());

	packet.sendSTAT_EOT();
}
void SrvPI::cmdGET(string pathname)
{
	// printf("SrvPI::cmdGET(string pathname) request\n");

	// string msg_o;
	// if (!combineAndValidatePath(GET, pathname, msg_o))
 //   	{
 //   		packet.sendSTAT_ERR(pathname + msg_o.c_str());
	// 	return;
 //   	}
	// string path = userRootDir + userRCWD + "/" + pathname;

	//std::cout << "cmdGET path[" << path << "]" << '\n';
	SrvDTP srvDTP(&(this->packet), this);
	srvDTP.sendFile(pathname.c_str());

	packet.sendSTAT_EOT();
}
 
void SrvPI::RGET_recurse(string srvpath, string clipath)
{
	DIR * dir= opendir(srvpath.c_str());
	char buf[MAXLINE];
	if(!dir)
	{
		// send STAT_ERR Response
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		// send STAT_OK
		packet.sendSTAT_OK();
	}

	struct dirent* e;
	if (srvpath.back() != '/')
	{
		srvpath += "/";
	}
	if (clipath.back() != '/')
	{
		clipath += "/";
	}

	while( (e = readdir(dir)) )
	{
		if(e->d_type == 4 && strcmp(e->d_name, ".") && strcmp(e->d_name, ".."))
		{
			packet.sendCMD_LMKDIR(clipath + e->d_name);
			recvOnePacket();
			if (packet.getTagid() == TAG_STAT)
			{
				if (packet.getStatid() == STAT_OK)
				{
					RGET_recurse(srvpath + e->d_name, clipath + e->d_name);
				} else if (packet.getStatid() == STAT_ERR)
				{
					Error::msg("error: mkdir %s", (clipath + e->d_name).c_str());
					return;
				}
				 
			} else {
				Error::msg("unknown tagid: %d", packet.getTagid());
				return;
			}
		}
		else if(e->d_type == 8)
		{
			string str = srvpath + e->d_name;
			str += DELIMITER;
			str += clipath + e->d_name;
			packet.sendCMD_GET(str);
			recvOnePacket();
			if (packet.getTagid() == TAG_CMD && packet.getCmdid() == GET)
			{
				cmdGET(packet.getSBody());
			} else {
				Error::msg("Error: cmdGET unknown tagid with statid");
				packet.print();
				return;
			}
		}
	}
	closedir(dir);
}

void SrvPI::RGET_iterate(string srvrootpath, string clirootpath)
{
	std::queue< pair<string, string > > dirQueue;
	dirQueue.push(pair<string , string >(srvrootpath, clirootpath));

	while(!dirQueue.empty())
	 {
		pair<string , string > dirPair = dirQueue.front();
		string srvpath = dirPair.first;
		string clipath = dirPair.second;

		// first create dir on client host
		packet.sendCMD_LMKDIR(clipath);
		recvOnePacket();
		if (packet.getTagid() == TAG_STAT)
		{
			if (packet.getStatid() == STAT_OK)
			{
				dirQueue.pop(); // client create dir successfully
			} else if (packet.getStatid() == STAT_ERR)
			{
				Error::msg("error: mkdir %s", clipath.c_str());
				return;
			} else {
				Error::msg("unknown statid: %d", packet.getStatid());
				return;
			}
			 
		} else {
			Error::msg("unknown tagid: %d", packet.getTagid());
			return;
		}
		// then iterate this dir
		DIR * dir= opendir(srvpath.c_str());
		char buf[MAXLINE];
		if(!dir)
		{
			// send STAT_ERR Response
			// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
			packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
			return;
		} else {
			// send STAT_OK
			packet.sendSTAT_OK();
		}

		struct dirent* e;
		if (srvpath.back() != '/')
		{
			srvpath += "/";
		}
		if (clipath.back() != '/')
		{
			clipath += "/";
		}

		while( (e = readdir(dir)) )
		{
			if(e->d_type == 4 && strcmp(e->d_name, ".") && strcmp(e->d_name, ".."))
			{
				dirQueue.push(pair<string , string >(srvpath + e->d_name, clipath + e->d_name));
			}
			else if(e->d_type == 8)
			{
				string str = srvpath + e->d_name;
				str += DELIMITER;
				str += clipath + e->d_name;
				packet.sendCMD_GET(str);
				recvOnePacket();
				if (packet.getTagid() == TAG_CMD && packet.getCmdid() == GET)
				{
					cmdGET(packet.getSBody());
				} else {
					Error::msg("Error: cmdGET unknown tagid with statid");
					packet.print();
					return;
				}
			}
		}
		closedir(dir);
		
	}
}

void SrvPI::cmdRGET()
{
	printf("RGET request\n");
	vector<string> paramVector; 
	split(packet.getSBody(), DELIMITER, paramVector);

	string msg_o;
	if (!combineAndValidatePath(RGET, paramVector[0], msg_o))
   	{
   		packet.sendSTAT_ERR(msg_o.c_str());
		return;
   	}

   	string srvpath = userRootDir + (userRCWD == "/" ? "/": userRCWD + "/") + paramVector[0];
   	string clipath;
   	vector<string> pathVector; 
   	if (paramVector.size() == 1)
   	{
   		split(paramVector[0], "/", pathVector);
   		clipath = pathVector.back();
   	} else {
   		clipath = paramVector[1];
   	}
	

	// first create target dir
	// packet.sendCMD_LMKDIR(pathVector.back());
	// recvOnePacket();
	// if (packet.getTagid() == TAG_STAT)
	// {
	// 	if (packet.getStatid() == STAT_OK)
	// 	{
	// 		// then transfer inside dirs and files
 //   			RGET_recurse(tmpDir, pathVector.back());
	// 	} else if (packet.getStatid() == STAT_ERR)
	// 	{
	// 		Error::msg("error: mkdir %s", pathVector.back().c_str());
	// 		return;
	// 	}
		 
	// } else {
	// 	Error::msg("unknown tagid: %d", packet.getTagid());
	// 	return;
	// }


	RGET_iterate(srvpath, clipath);
	// packet.sendCMD_LMKDIR(pathVector.back());
	// recvOnePacket();
	// if (packet.getTagid() == TAG_STAT)
	// {
	// 	if (packet.getStatid() == STAT_OK)
	// 	{
	// 		// then transfer inside dirs and files
 //   			RGET_recurse(tmpDir, pathVector.back());
	// 	} else if (packet.getStatid() == STAT_ERR)
	// 	{
	// 		Error::msg("error: mkdir %s", pathVector.back().c_str());
	// 		return;
	// 	}
		 
	// } else {
	// 	Error::msg("unknown tagid: %d", packet.getTagid());
	// 	return;
	// }
	packet.sendSTAT_EOT();		
}

void SrvPI::cmdPUT()
{
	printf("PUT request\n");
	
	vector<string> paramVector; 
	split(packet.getSBody(), DELIMITER, paramVector);
	for (auto it = paramVector.cbegin(); it != paramVector.cend(); ++it)
           std::cout << it->size() << "paramVector: " << *it << std::endl;
	string path;
	string userinput;
	if (paramVector.size() == 1){
		userinput = paramVector[0];
		vector<string> pathVector; 
		split(userinput, "/", pathVector); 
		path = userRootDir + (userRCWD == "/" ? "/": userRCWD + "/") + pathVector.back();
	} else if (paramVector.size() == 2){
		userinput = paramVector[1];
		path = userRootDir + (userRCWD == "/" ? "/": userRCWD + "/") + paramVector[1];
	}

	
	string msg_o;
	if (!combineAndValidatePath(PUT, userinput, msg_o))
   	{
   		packet.sendSTAT_CFM(msg_o.c_str());
   		recvOnePacket();
   		if(packet.getTagid() == TAG_STAT && packet.getStatid() == STAT_CFM) {
   			packet.print();
			if (packet.getSBody() == "y")
			{
				std::cout << "packet.getSBody() == y" << '\n';
				SrvDTP srvDTP(&(this->packet), this);
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
   		std::cout << "**************cmdPUT path[" << path << "]" << '\n';
		SrvDTP srvDTP(&(this->packet), this);
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
   		packet.sendSTAT_ERR(msg_o.c_str());
		return;
   	}
	string tmpDir = userRootDir + (userRCWD == "/" ? "/": userRCWD + "/") + paramVector[0];
	DIR * dir= opendir(tmpDir.c_str());
	if(!dir)
	{
		// send STAT_ERR Response
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		// send STAT_OK
		packet.sendSTAT_OK();
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
			packet.sendDATA_LIST(0, 0, sbody.size(), sbody.c_str());
			sbody.clear();
		}
		sbody += buf;
		
	}
	if (!sbody.empty())
	{
		if ( cnt !=0 && (cnt % 5) == 0)
		{
			sbody.pop_back(); // remove '\n'
		}
		packet.sendDATA_LIST(0, 0, sbody.size(), sbody.c_str());
	}

	packet.sendSTAT_EOT();

}

void SrvPI::cmdCD()
{
	printf("CD request\n");
	vector<string> paramVector; 
	split(packet.getSBody(), DELIMITER, paramVector);
	string msg_o;
   	if (!combineAndValidatePath(CD, paramVector[0], msg_o))
   	{
   		packet.sendSTAT_ERR(msg_o.c_str());
		return;
   	} else {
		packet.sendSTAT_OK("CWD: ~" + userRCWD);
		return;
   	}
	// if( (n = chdir(newAbsDir.c_str())) == -1)
	// {
	// 	// send STAT_ERR Response
	// 	// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
	// 	packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
	// 	return;
	// } else {
	// 	// send STAT_OK
	// 	snprintf(buf, MAXLINE, "server: change CWD to [%s]", packet.ps->body);
	// 	packet.sendSTAT_OK(buf);
	// }
	//packet.sendSTAT_EOT();
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
   		packet.sendSTAT_ERR(msg_o.c_str());
		return;
   	} else {
   		string path = userRootDir + (userRCWD == "/" ? "/": userRCWD + "/") + paramVector[0];
   		char buf[MAXLINE]; 
   		if( remove(path.c_str()) !=0 )
		{
			// send STAT_ERR Response 
			// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
			packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
			return;
		} else {
			// send STAT_OK
			packet.sendSTAT_OK(paramVector[0] + " is removed");
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
	// 		packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
	// 		return;
	// 	} else {
	// 		// send STAT_OK
	// 		packet.sendSTAT_OK(packet.getSBody() + "is removed");
	// 		return;
	// 	}
	// } else if (S_ISDIR(statBuf.st_mode)){
	// 	packet.sendSTAT_ERR("rm: cannot remove '" + packet.getSBody() + "': Is a directory");
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
			packet.sendSTAT_OK((userRootDir + userRCWD).c_str());
		} else {
			packet.sendSTAT_ERR("command format error");
		}
		
	} else {
		packet.sendSTAT_OK(("~" + userRCWD).c_str());
	}
	

	// char buf[MAXLINE];
	// if( !getcwd(buf, MAXLINE))
	// {
	// 	// send STAT_ERR Response
	// 	// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
	// 	packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
	// 	return;
	// } else {
	// 	// send STAT_OK
	// 	packet.sendSTAT_OK(userRCWD.c_str());
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
   		packet.sendSTAT_ERR(msg_o.c_str());
		return;
   	} else {
   		string path = userRootDir + (userRCWD == "/" ? "/": userRCWD + "/") + paramVector[0];
   		char buf[MAXLINE]; 
   		if(mkdir(path.c_str(), 0777) == -1){
   			// send STAT_ERR Response
			// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
			msg_o += "system call (mkdir): ";
			msg_o += strerror_r(errno, buf, MAXLINE);
			packet.sendSTAT_ERR(msg_o.c_str());
   		}else {
			// send STAT_OK
			packet.sendSTAT_OK(paramVector[0] + " created");
		}
	}
}
void SrvPI::rmdirDFS()
{
	DIR *cur_dir = opendir(".");
	struct dirent *ent = NULL;
	struct stat st;

	if (!cur_dir)
	{
		Error::ret("opendir");
		return;
	}

	while ((ent = readdir(cur_dir)) != NULL)
	{
		stat(ent->d_name, &st);
	
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
		{
			continue;
		}

		if (S_ISDIR(st.st_mode))
		{
			if( chdir(ent->d_name) == -1)
			{
				Error::sys("CliPI::rmdirDFS chdir");
				return;
			} else {
				this->rmdirDFS();
				if( chdir("..") == -1)
				{
					Error::sys("CliPI::rmdirDFS chdir(..)");
					return;
				} 
			}
		}

		remove(ent->d_name);
	}
	
	closedir(cur_dir);
}

void SrvPI::removeDir(const char *path_raw, bool removeSelf)
{
	char old_path[MAXLINE];

	if (!path_raw)
	{
		return;
	}

	if ( !getcwd(old_path, MAXLINE))
	{
		Error::sys("getcwd");
	}
	
	if (chdir(path_raw) == -1)
	{
		fprintf(stderr, "not a dir or access error\n");
		Error::sys("removeDir chdir(path_raw)");
		return;
	}

	printf("path_raw : %s\n", path_raw);
	this->rmdirDFS();
	if (chdir(old_path) == -1)
	{
		Error::sys("removeDir chdir(path_raw)");
		return;
	}

	if (removeSelf)
	{
		unlink(old_path); 
	}
}
void SrvPI::cmdRMDIR()
{
	printf("RMDIR request\n");
	vector<string> paramVector; 
	split(packet.getSBody(), DELIMITER, paramVector);
	string msg_o;
   	if (!combineAndValidatePath(RMDIR, paramVector[0], msg_o))
   	{
   		packet.sendSTAT_ERR(msg_o.c_str());
		return;
   	} else {
   		string path = userRootDir + (userRCWD == "/" ? "/": userRCWD + "/") + paramVector[0];
   		string shellCMD = "rm -rf " + path;
		if (system(shellCMD.c_str()) == -1) {
			char buf[MAXLINE];
			packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
		} else {
			// send STAT_OK
			packet.sendSTAT_OK("Dir" + paramVector[0] + " removed");
		}
		//packet.sendSTAT_OK("rmdir: not finished");
	}
}

void SrvPI::cmdSHELL()
{
	printf("SHELL request\n");

	char buf[MAXLINE];
	vector<string> paramVector; 
	split(packet.getSBody(), DELIMITER, paramVector);

	string curpath = userRootDir + (userRCWD == "/" ? "/": userRCWD + "/");
	string shellcmdstring = "cd " + curpath + "; ";
	auto it = paramVector.begin();
	shellcmdstring +=  *it; // first get command name
	for (++it; it != paramVector.end(); ++it)
	{
		if ((*it)[0] == '-')
		{
			shellcmdstring += " " + *it;
		} else {
			string msg_o;
		   	if (!combineAndValidatePath(SHELL, *it, msg_o))
		   	{
		   		packet.sendSTAT_ERR(msg_o.c_str());
				return;
		   	} else {
		   		string path = userRootDir + (userRCWD == "/" ? "/": userRCWD + "/") + *it;
				shellcmdstring += " " + path;
			}
		}
		
	}

	shellcmdstring += " 2>&1";
	cout<< shellcmdstring << endl;
	FILE *fp = popen(shellcmdstring.c_str(), "r");
    if (fp == NULL) 
    {
		packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
		return;
    }

    char body[PBODYCAP] = {0};
    int n;
    packet.sendSTAT_OK();
    while ( (n = fread(body, sizeof(char), PBODYCAP, fp)) > 0)
	{
		packet.sendDATA_TEXT(n, body);
	}
	pclose(fp);

	packet.sendSTAT_EOT();
}

bool SrvPI::combineAndValidatePath(uint16_t cmdid, string userinput, string & msg_o)
{
	if (userinput.front() == '/')
	{
		string newAbsDir = userinput;
		if (newAbsDir.substr(0, userRootDir.size()) != userRootDir)
	   	{
			//std::cout << "Permission denied: " << newAbsDir << '\n';
			msg_o = "Permission denied: " + newAbsDir;
			return false;
	   	} else { 
		   	if (cmdid == RMDIR && newAbsDir == userRootDir)
		   	{
		   	 	msg_o = "Permission denied: " + newAbsDir;
				return false;
		   	} 
	   		return cmdPathProcess(cmdid, newAbsDir, msg_o);
	   	}
	}
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
	   	if (cmdid == RMDIR && newAbsDir == userRootDir)
	   	{
	   	 	msg_o = "Permission denied: " + newAbsDir;
			return false;
	   	} 
   		return cmdPathProcess(cmdid, newAbsDir, msg_o);
   	}
}

bool SrvPI::cmdPathProcess(uint16_t cmdid, string newAbsDir, string & msg_o)
{
	string rpath = newAbsDir.substr(userRootDir.size(), newAbsDir.size() - userRootDir.size());
    if (rpath.empty())
	{
		rpath = "/";
	}
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
					msg_o = "get: '" + newAbsDir + "' is a directory";
					return false;
			    } else {
			    	msg_o = "get: '" + newAbsDir + "' not a regular file";
					return false;
			    }
				
			} else { // stat error
				msg_o = newAbsDir + strerror_r(errno, buf, MAXLINE);
				return false;
			}
			break;	
		}
		case RGET:
		{
			struct stat statBuf;
	   		char buf[MAXLINE];
		    int n = stat(newAbsDir.c_str(), &statBuf);
		    if(!n) // stat call success
			{	
				if (S_ISREG(statBuf.st_mode)){
					msg_o = "rget: '" + newAbsDir + "' is a regular file";
					return false;
			    } else if (S_ISDIR(statBuf.st_mode)){
					return true;
			    } else {
			    	msg_o = "rget: '" + newAbsDir + "' not a directory";
					return false;
			    }
				
			} else { // stat error
				msg_o =newAbsDir + strerror_r(errno, buf, MAXLINE);
				return false;
			}
			break;	
		}
		case PUT:
		{
			if ((access(newAbsDir.c_str(), F_OK)) == 0) {
				// send STAT_ERR Response
				string rpath = newAbsDir.substr(userRootDir.size(), newAbsDir.size() - userRootDir.size());
				if (rpath.empty())
				{
					rpath = "/";
				}
				msg_o = "File '~" + rpath + "' already exists, overwrite ? (y/n) ";
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
		    string rpath = newAbsDir.substr(userRootDir.size(), newAbsDir.size() - userRootDir.size());
		    if (rpath.empty())
			{
				rpath = "/";
			}
		    if(!n) // stat call success
			{	
				if (S_ISREG(statBuf.st_mode)){
					return true;
			    } else if (S_ISDIR(statBuf.st_mode)){
					msg_o = "rm: '~" + rpath + "' is a directory";
					return false;
			    } else {
			    	msg_o = "rm: '~" + rpath + "' not a regular file";
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
		case RMDIR:
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
					msg_o = "rmdir: '~" + rpath + "' is a regular file";
					return false;
			    } else if (S_ISDIR(statBuf.st_mode)){
					return true;
			    } else {
			    	msg_o = "rmdir: '~" + rpath + "' not a directory";
					return false;
			    }
				
			} else { // stat error
				msg_o = strerror_r(errno, buf, MAXLINE);
				return false;
			}
			break;
		}
		case SHELL:
		{
			if ((access(newAbsDir.c_str(), F_OK)) == 0) 
			{
				return true;
			} else {
				msg_o = "~" + rpath + ": No such file or directory";
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

int SrvPI::getConnfd()
{
	return connfd;
}

SrvPI::~SrvPI()
{

}
void SrvPI::saveUserState()
{
	map<string, string> updateParamMap = {  {"RCWD", userRCWD} };
	db.update("user", userID, updateParamMap);
	std::cout<< "\n\033[32msave user state ok\033[0m" << std::endl;
}