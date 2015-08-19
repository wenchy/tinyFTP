#include    "clipi.h"


std::map<string, string> CliPI::helpMap = {	//{"USER",    "user 	username"},
                                            //{"PASS",    "pass 	password"},

                                            {"GET",     "get [remote-file] [local-file]"},
                                            {"PUT",     "put [local-file] [remote-file]"},
                                            {"LS",      "ls [remote-dir]"},
                                            {"LLS",     "lls same as local ls"},
                                            {"CD",      "cd [remote-dir]"},
                                            {"LCD",     "lcd [local-dir]"},
                                            {"RM",      "rm [remote-file]"},
                                            {"LRM",     "lrm same as local rm"},
                                            {"PWD",     "pwd [-a]"},
                                            {"LPWD",    "lpwd same as local pwd"},
                                            {"MKDIR",   "mkdir [remote-dir]"},
                                            {"LMKDIR",  "lmkdir same as local mkdir"},
                                            {"QUIT",    "quit"},
                                            {"HELP",    "help [cmd]"},

                                            {"MGET",    "mget [file]..."},
                                            {"MPUT",    "mput [file]..."},
                                            {"RGET",    "rget [dir]"},
                                            {"RPUT",    "rput [dir]"},
                                            {"RMDIR",   "rmdir [dir]"},
                                            {"SHELL",   "shell [remote-shell-cmd]"},

                                            //{"BINARY",  "binary"},
                                            //{"ASCII",   "ascii"}  	
                                        								};

CliPI::CliPI(const char *host): packet(this)
{
    Socket cliSocket(CLI_SOCKET, host, CTRPORT);
    connfd = cliSocket.init();
    connSockStream.init(connfd);
}

bool CliPI::recvOnePacket()
{
	int n;
	packet.reset(NPACKET);
	if ( (n = connSockStream.readn(packet.getPs(), PACKSIZE)) == 0)
	{
		this->saveUserState();
		Socket::tcpClose(connfd);
		Error::quit("server terminated prematurely");
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

bool CliPI::sendOnePacket(PacketStruct * ps, size_t nbytes)
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
				Error::quit_pthread("server terminated prematurely");
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

void CliPI::run(uint16_t cmdid, std::vector<string> & paramVector)
{
	switch(cmdid)
	{
		case USER:
			cmdUSER(paramVector);
			break;
		case PASS:
			cmdPASS(paramVector);
			break;
		case GET:
			cmdGET(paramVector);
			break;
		case RGET:
			cmdRGET(paramVector);
			break;
		case PUT:
			cmdPUT(paramVector);
			break;
		case LS:
			cmdLS(paramVector);
			break;
		case LLS:
			cmdLLS(paramVector);
			break;
		case CD:
			cmdCD(paramVector);
			break;
		case LCD:
			cmdLCD(paramVector);
			break;
		case RM:
			cmdRM(paramVector);
			break;
		case LRM:
			cmdLRM(paramVector);
			break;	
		case PWD:
			cmdPWD(paramVector);
			break;
		case LPWD:
			cmdLPWD(paramVector);
			break;
		case MKDIR:
			cmdMKDIR(paramVector);
			break;
		case LMKDIR:
			cmdLMKDIR(paramVector);
			break;
		case RMDIR:
			cmdRMDIR(paramVector);
			break;
		case SHELL:
			cmdSHELL(paramVector);
			break;
		case LSHELL:
			cmdLSHELL(paramVector);
			break;
		case QUIT:
			cmdQUIT(paramVector);
			break;
		case HELP:
			cmdHELP(paramVector);
			break;
		default:
			Error::msg("Client: Sorry! this command function not finished yet.\n");
			break;
	}
	
}
void CliPI::split(std::string src, std::string token, vector<string>& vect)   
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

string CliPI::getEncodedParams(std::vector<string> & paramVector)
{
	string encodedParams;
	if(!paramVector.empty())
	{
		vector<string>::iterator iter=paramVector.begin();
		encodedParams += *iter;
		for (++iter; iter!=paramVector.end(); ++iter)
	   	{
	   		encodedParams += DELIMITER + *iter;
	   	}
	} 

	return encodedParams;
}

bool CliPI::cmdUSER(std::vector<string> & paramVector)
{
	if(paramVector.size() != 1)
	{
		Error::msg("Usage: [username]");
		return false;
	} else {
		packet.sendCMD(USER, getEncodedParams(paramVector));
		// first receive response
		recvOnePacket();
		if (packet.getTagid() == TAG_STAT) {
			if (packet.getStatid() == STAT_OK) {
				return true;
			} else if (packet.getStatid() == STAT_ERR){
				cerr<< packet.getSBody() << endl;
				return false;
			} else {
				Error::msg("CliPI::cmdUSER: unknown statid %d", packet.getStatid());
				return false;
			}
			
		} else {
			Error::msg("CliPI::cmdUSER: unknown tagid %d", packet.getTagid());
			return false;
		}
	}
 
}

bool CliPI::cmdPASS(std::vector<string> & paramVector)
{
	if(paramVector.size() != 2)
	{
		Error::msg("Usage: [password]");
		for (vector<string>::iterator iter=paramVector.begin(); iter!=paramVector.end(); ++iter)
	   	{
	    	std::cout << *iter << '\n';
	   	}
		return false;
	}

	packet.sendCMD(PASS, getEncodedParams(paramVector));

	// first receive response
	recvOnePacket();
	if (packet.getTagid() == TAG_STAT) {
		if (packet.getStatid() == STAT_OK) {
			// init userID, same as session id
			char buf[MAXLINE];
			snprintf (buf, MAXLINE, "%u", packet.getSesid());
			userID = buf;
			cout<< packet.getSBody() << endl;
			return true;
		} else if (packet.getStatid() == STAT_ERR){
			cerr<< packet.getSBody() << endl;
			return false;
		} else {
			Error::msg("CliPI::cmdPASS: unknown statid %d", packet.getStatid());
			return false;
		}
		
	} else {
		Error::msg("CliPI::cmdPASS: unknown tagid %d", packet.getTagid());
		return false;
	}
 
}

void CliPI::cmdGET(std::vector<string> & paramVector)
{
	if(paramVector.size() > 2)
	{
		std::cout << "Usage: " << helpMap["GET"] << std::endl;
		return;
	}

	string pathname;
	char buf[MAXLINE];
	if (paramVector.size() == 1){
		vector<string> pathVector; 
		split(paramVector[0], "/", pathVector);
		pathname = pathVector.back();
	} else if (paramVector.size() == 2){
		pathname = paramVector[1];
	}
	
	if ((access(pathname.c_str(), F_OK)) == 0) {
		snprintf(buf, MAXLINE, "File [%s] already exists, overwrite ? (y/n) ", pathname.c_str());
		if(!confirmYN(buf))
		{
			return;
		}
	}
	FILE *fp;
	if ( (fp = fopen(pathname.c_str(), "wb")) == NULL) {
		Error::msg("%s", strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		packet.sendCMD(GET, getEncodedParams(paramVector));
	}

    // pathname exist on server? need test
    CliDTP cliDTP(&(this->packet), this);
	cliDTP.recvFile(pathname.c_str(), fp);
 
}


void CliPI::cmdGET(string srvpath, string clipath)
{
	//printf("GET request: srvpath:\n");
	//cout << "GET request: " << "srvpath=" << srvpath << " clipath=" << clipath << endl;

	char buf[MAXLINE];
	
	FILE *fp;
	if ((access(clipath.c_str(), F_OK)) == 0) {
		snprintf(buf, MAXLINE, "File [%s] already exists, overwrite ? (y/n) ", clipath.c_str());
		if(!confirmYN(buf))
		{
			packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
			return;
		}
	}

	if ( (fp = fopen(clipath.c_str(), "wb")) == NULL) {
		Error::msg("%s", strerror_r(errno, buf, MAXLINE));
		packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		packet.sendCMD_GET(srvpath);
	}

    CliDTP cliDTP(&(this->packet), this);
	cliDTP.recvFile(clipath.c_str(), fp);
 
}

bool CliPI::confirmYN(const char * prompt)
{
	string inputline, word;
	vector<string> paramVector;
	while (printf("%s", prompt), getline(std::cin, inputline))
	{
		paramVector.clear();
	    std::istringstream is(inputline);
	    while(is >> word)
	        paramVector.push_back(word);

	    // if user enter nothing, assume special anonymous user
	    if (paramVector.size() == 1){
	       if (paramVector[0] == "y"){
	       		return true;
	       } else if (paramVector[0] == "n"){
	       		return false;
	       } else {
	       		continue;
	       }
	    } else {
	        continue;
	    }
	}
	return false;
}

void CliPI::rmdirDFS()
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

void CliPI::removeDir(const char *path_raw, bool removeSelf)
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

void CliPI::cmdRGET(std::vector<string> & paramVector)
{
	if(paramVector.size() > 2)
	{
		std::cout << "Usage: " << helpMap["RGET"] << std::endl;
		return;
	}
	string pathname;
	char buf[MAXLINE];
	if (paramVector.size() == 1){
		vector<string> pathVector; 
		split(paramVector[0], "/", pathVector);
		pathname = pathVector.back();
	} else if (paramVector.size() == 2){
		pathname = paramVector[1];
	}

	if ((access(pathname.c_str(), F_OK)) == 0) { // already exists
		snprintf(buf, MAXLINE, "[%s] already exists, overwrite ? (y/n) ", pathname.c_str());
		if(!confirmYN(buf))
		{
			return;
		} else {
			// yes to overwite
			//removeDir(paramVector[0].c_str(), false);
	   		string shellCMD = "rm -rf " + pathname;
			if (system(shellCMD.c_str()) == -1) {
				printf("%s\n", strerror_r(errno, buf, MAXLINE));
				return;
			} else {
				// OK
				printf("Dir '%s' emptied and removed\n",  pathname.c_str());
			}
		}
	}

	packet.sendCMD(RGET, getEncodedParams(paramVector));
	
	CliDTP cliDTP(&(this->packet), this);

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
						vector<string> paramVector; 
						split(packet.getSBody(), DELIMITER, paramVector);
						cmdGET(paramVector[0], paramVector[1]);
						break;
					}
					case LMKDIR:
					{
						if (cmdLMKDIR(packet.getSBody()))
						{
							packet.sendSTAT_OK();
						} else {
							packet.sendSTAT_ERR();
							return;
						}
						break;
					}
					default:
					{
						Error::msg("unknown cmdid: %d", packet.getCmdid());
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
						//cout << packet.getSBody() <<endl;
						break;
					}
					case STAT_ERR:
					{
						cerr << packet.getSBody() <<endl;
						return;
					}
					// case STAT_EOF:
					// {
					// 	cout << packet.getSBody() <<endl;
					// 	break;
					// }
					case STAT_EOT:
					{
						cout << packet.getSBody() <<endl;
						return;
					}
					default:
					{
						Error::msg("unknown statid: %d", packet.getStatid());
						break;
					}
				}
				break;
			}
			case TAG_DATA:
			{
				switch(packet.getDataid())
				{
					// case DATA_FILE:
					// {
					// 	cout << "DATA_FILE" << packet.getSBody() <<endl;
					// 	break;
					// }
					case DATA_NAME:
					{
						cout << "DATA_NAME" << packet.getSBody() <<endl;
						break;
					}
					default:
					{
						Error::msg("unknown statid: %d", packet.getStatid());
						break;
					}
				}
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

void CliPI::cmdPUT(std::vector<string> & paramVector)
{
	if(paramVector.size() > 2)
	{
		std::cout << "Usage: " << helpMap["PUT"] << std::endl;
		return;
	}

	char pathname[MAXLINE];
	char buf[MAXLINE];
	uint32_t nslice = 0;
	uint32_t sindex = 0;

	strcpy(pathname,paramVector[0].c_str()); 
	struct stat statBuf;
    int n = stat(paramVector[0].c_str(), &statBuf);
    if(!n) // stat call success
	{	
		if (S_ISREG(statBuf.st_mode)){
			;
	    } else if (S_ISDIR(statBuf.st_mode)){
			cout << "put: cannot upload [" << paramVector[0] << "]: Is a directory" << endl;
			return;
	    } else {
	    	cout << "put: [" << paramVector[0] << "] not a regular file or directory" << endl;
			return;
	    }
		
	} else { // stat error
		Error::msg("%s", strerror_r(errno, buf, MAXLINE));
		return;
	}

	
	FILE *fp;
	if ( (fp = fopen(pathname, "rb")) == NULL)
	{
		Error::msg("%s", strerror_r(errno, buf, MAXLINE));
		return;
	} else if ( (n = getFileNslice(pathname, &nslice)) < 0)  {
		if ( n == -2) {
			Error::msg("Too large file size.", buf);
		} else {
			Error::msg("File stat error.");
		}
		return;
	} else {
		packet.sendCMD(PUT, getEncodedParams(paramVector));
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
					case LMKDIR:
					{
						break;
					}
					default:
					{
						Error::msg("unknown cmdid: %d", packet.getCmdid());
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
						CliDTP cliDTP(&(this->packet), this);
						cliDTP.sendFile(pathname, fp, nslice, sindex);
						packet.sendSTAT_EOT();
						return;
					}
					case STAT_BPR:
					{
						cout << packet.getSBody() <<endl;
						vector<string> paramVector; 
						split(packet.getSBody(), DELIMITER, paramVector);
						sindex = std::stoul(paramVector[1]);
						break;
					}
					case STAT_CFM:
					{
						if(confirmYN(packet.getSBody().c_str()))
						{
							packet.sendSTAT_CFM("y");
						} else {
							packet.sendSTAT_CFM("n");
							return;
						}
						break;
					}
					case STAT_ERR:
					{
						cerr << packet.getSBody() <<endl;
						return;
					}
					case STAT_EOF:
					{
						cout << packet.getSBody() <<endl;
						break;
					}
					case STAT_EOT:
					{
						cout << packet.getSBody() <<endl;
						return;
					}
					default:
					{
						Error::msg("unknown statid: %d", packet.getStatid());
						break;
					}
				}
				break;
			}
			case TAG_DATA:
			{
				switch(packet.getDataid())
				{
					case DATA_FILE:
					{
						cout << "DATA_FILE" << packet.getSBody() <<endl;
						break;
					}
					default:
					{
						Error::msg("unknown statid: %d", packet.getStatid());
						break;
					}
				}
				break;
			}
			default:
			{
				Error::msg("unknown tagid: %d", packet.getTagid());
				break;
			}
		}
	}


	// while (recvOnePacket())
	// {
	// 	if (packet.getTagid() == TAG_STAT) {
	// 		if (packet.getStatid() == STAT_OK) {
	// 			//cout << packet.getSBody() <<endl;
	// 			// must contain sesssion id
	// 			CliDTP cliDTP(&(this->packet), this);
	// 			cliDTP.sendFile(pathname, fp, nslice);
	// 			break;
	// 		} else if (packet.getStatid() == STAT_BPR) {
	// 			CliDTP cliDTP(&(this->packet), this);
	// 			cliDTP.sendFile(pathname, fp, nslice);
	// 			break;
	// 		} else if (packet.getStatid() == STAT_CFM) {
	// 			if(confirmYN(packet.getSBody().c_str()))
	// 			{
	// 				packet.sendSTAT_CFM("y");
	// 				continue;
	// 			} else {
	// 				packet.sendSTAT_CFM("n");
	// 				return;
	// 			}
	// 		} else if (packet.getStatid() == STAT_ERR) {
	// 			cerr << packet.getSBody() <<endl;
	// 			return;
	// 		} else {
	// 			Error::msg("unknown statid %d", packet.getStatid());
	// 			packet.print();
	// 			return;
	// 		}
			
	// 	} else {
	// 		Error::msg("unknown tagid %d", packet.getTagid());
	// 		packet.print();
	// 		return;
	// 	}
	// }

	//printf("EOT %s\n", pathname);
	//packet.sendSTAT_EOT();
	
}
void CliPI::cmdLS(std::vector<string> & paramVector)
{
	if(paramVector.size() > 1)
	{
		std::cout << "Usage: " << helpMap["LS"] << std::endl;
		return;
	}
	
	packet.sendCMD(LS, getEncodedParams(paramVector));

	// first receive response
	recvOnePacket();
	if (packet.getTagid() == TAG_STAT) {
		if (packet.getStatid() == STAT_OK) {
			cout<< packet.getSBody() << endl;
		} else if (packet.getStatid() == STAT_ERR){
			cerr<< packet.getSBody() << endl;
			return;
		} else {
			Error::msg("unknown statid %d", packet.getStatid());
			return;
		}
		
	} else {
		Error::msg("unknown tagid %d", packet.getTagid());
		return;
	}

	while(1) 
	{
		recvOnePacket();
		if (packet.getTagid() == TAG_DATA) {
			cout<< packet.getSBody() << endl;
			
		} else if (packet.getTagid() == TAG_STAT && packet.getStatid() == STAT_EOT){
			cout<< packet.getSBody() << endl;
			break;
		}
	}
}

void CliPI::cmdLLS(std::vector<string> & paramVector)
{
	string shellCMD = "ls --color=auto";
	for (auto it = paramVector.begin(); it != paramVector.end(); ++it){
       	//std::cout << *it << std::endl;
       	shellCMD += " " + *it;
	}
	if (system(shellCMD.c_str()) == -1) {
		char buf[MAXLINE];
		std::cout << "system(): " << strerror_r(errno, buf, MAXLINE) << std::endl;
	}
}

void CliPI::cmdCD(std::vector<string> & paramVector)
{
	if(paramVector.size() != 1)
	{
		std::cout << "Usage: " << helpMap["CD"] << std::endl;
		return;
	}

	packet.sendCMD(CD, getEncodedParams(paramVector));

	// first receive response
	recvOnePacket();
	if (packet.getTagid() == TAG_STAT) {
		if (packet.getStatid() == STAT_OK) {
			cout << packet.getSBody() <<endl;
			return;
		} else if (packet.getStatid() == STAT_ERR){
			cerr << packet.getSBody() <<endl;
			return;
		} else {
			Error::msg("unknown statid %d", packet.getStatid());
			return;
		}
		
	} else {
		Error::msg("unknown tagid %d", packet.getTagid());
		return;
	}
 
}

void CliPI::cmdLCD(std::vector<string> & paramVector)
{
	if(paramVector.size() != 1)
	{
		std::cout << "Usage: " << helpMap["LCD"] << std::endl;
		return;
	}

	int n;
	//char buf[MAXLINE];
	if( (n = chdir(paramVector[0].c_str())) == -1)
	{
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		Error::ret("lcd");
		return;
	}
}

void CliPI::cmdRM(std::vector<string> & paramVector)
{
	if(paramVector.size() != 1)
	{
		std::cout << "Usage: " << helpMap["RM"] << std::endl;
		return;
	}

	packet.sendCMD(RM, getEncodedParams(paramVector));

	// first receive response
	recvOnePacket();
	if (packet.getTagid() == TAG_STAT) {
		if (packet.getStatid() == STAT_OK) {
			cout << packet.getSBody() <<endl;
			return;
		} else if (packet.getStatid() == STAT_ERR){
			cerr << packet.getSBody() <<endl;
			return;
		} else {
			Error::msg("unknown statid %d", packet.getStatid());
			return;
		}
		
	} else {
		Error::msg("unknown tagid %d", packet.getTagid());
		return;
	}
	
}

void CliPI::cmdLRM(std::vector<string> & paramVector)
{
	string shellCMD = "rm";
	for (auto it = paramVector.begin(); it != paramVector.end(); ++it){
       	//std::cout << *it << std::endl;
       	shellCMD += " " + *it;
	}
	if (system(shellCMD.c_str()) == -1) {
		char buf[MAXLINE];
		std::cout << "system(): " << strerror_r(errno, buf, MAXLINE) << std::endl;
	}
}

void CliPI::cmdPWD(std::vector<string> & paramVector)
{
	if(paramVector.size() > 1)
	{
		std::cout << "Usage: " << helpMap["PWD"] << std::endl;
		return;
	} else if (paramVector.size() == 1 && paramVector[0] != "-a")
	{
		std::cout << "Usage: " << helpMap["PWD"] << std::endl;
		return;
	}

	packet.sendCMD(PWD, getEncodedParams(paramVector));

	// first receive response
	recvOnePacket();
	if (packet.getTagid() == TAG_STAT) {
		if (packet.getStatid() == STAT_OK) {
			cout << packet.getSBody() <<endl;
			return;
		} else if (packet.getStatid() == STAT_ERR){
			cerr << packet.getSBody() <<endl;
			return;
		} else {
			Error::msg("unknown statid %d", packet.getStatid());
			return;
		}
		
	} else {
		Error::msg("unknown tagid %d", packet.getTagid());
		return;
	}
}

void CliPI::cmdLPWD(std::vector<string> & paramVector)
{
	string shellCMD = "pwd";
	for (auto it = paramVector.begin(); it != paramVector.end(); ++it){
       	//std::cout << *it << std::endl;
       	shellCMD += " " + *it;
	}
	if (system(shellCMD.c_str()) == -1) {
		char buf[MAXLINE];
		std::cout << "system(): " << strerror_r(errno, buf, MAXLINE) << std::endl;
	}
}

void CliPI::cmdMKDIR(std::vector<string> & paramVector)
{
	printf("MKDIR request\n");
	if(paramVector.size() != 1)
	{
		std::cout << "Usage: " << helpMap["MKDIR"] << std::endl;
		return;
	}

	packet.sendCMD(MKDIR, getEncodedParams(paramVector));

	// first receive response
	recvOnePacket();
	if (packet.getTagid() == TAG_STAT) {
		if (packet.getStatid() == STAT_OK) {
			cout << packet.getSBody() <<endl;
			return;
		} else if (packet.getStatid() == STAT_ERR){
			cerr << packet.getSBody() <<endl;
			return;
		} else {
			Error::msg("unknown statid %d", packet.getStatid());
			return;
		}
		
	} else {
		Error::msg("unknown tagid %d", packet.getTagid());
		return;
	}
}


bool CliPI::cmdLMKDIR(string path)
{
	//printf("LMKDIR(string path) request\n");

	char buf[MAXLINE]; 
	if(mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1){
		printf("\033[31mmkdir [%s] failed: %s\033[0m\n", path.c_str(), strerror_r(errno, buf, MAXLINE));
		return false;
	}else {
		printf("\033[35mDir [%s] created: \033[0m\n", path.c_str());
		return true;
	}

}

void CliPI::cmdLMKDIR(std::vector<string> & paramVector)
{
	string shellCMD = "mkdir";
	for (auto it = paramVector.begin(); it != paramVector.end(); ++it){
       	//std::cout << *it << std::endl;
       	shellCMD += " " + *it;
	}
	if (system(shellCMD.c_str()) == -1) {
		char buf[MAXLINE];
		std::cout << "system(): " << strerror_r(errno, buf, MAXLINE) << std::endl;
	}
}

void CliPI::cmdRMDIR(std::vector<string> & paramVector)
{
	if(paramVector.size() != 1)
	{
		std::cout << "Usage: " << helpMap["RMDIR"] << std::endl;
		return;
	}

	packet.sendCMD(RMDIR, getEncodedParams(paramVector));

	// first receive response
	recvOnePacket();
	if (packet.getTagid() == TAG_STAT) {
		if (packet.getStatid() == STAT_OK) {
			cout << packet.getSBody() <<endl;
			return;
		} else if (packet.getStatid() == STAT_ERR){
			cerr << packet.getSBody() <<endl;
			return;
		} else {
			Error::msg("unknown statid %d", packet.getStatid());
			return;
		}
		
	} else {
		Error::msg("unknown tagid %d", packet.getTagid());
		return;
	}
}

void CliPI::cmdLSHELL(std::vector<string> & paramVector)
{
	string shellcmdstring;
	for (auto it = paramVector.begin(); it != paramVector.end(); ++it){
       	//std::cout << *it << std::endl;
       	shellcmdstring += " " + *it;
	}
	if (system(shellcmdstring.c_str()) == -1) {
		char buf[MAXLINE];
		std::cout << "system(): " << strerror_r(errno, buf, MAXLINE) << std::endl;
	}
}

void CliPI::cmdSHELL(std::vector<string> & paramVector)
{
	if(paramVector.size() == 0)
	{
		std::cout << "Usage: " << helpMap["SHELL"] << std::endl;
		return;
	}

	packet.sendCMD(SHELL, getEncodedParams(paramVector));


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
					case LMKDIR:
					{
						break;
					}
					default:
					{
						Error::msg("unknown cmdid: %d", packet.getCmdid());
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
						cout << packet.getSBody() <<endl;
						break;
					}
					case STAT_ERR:
					{
						cerr << packet.getSBody() <<endl;
						return;
					}
					case STAT_EOF:
					{
						cout << packet.getSBody() <<endl;
						break;
					}
					case STAT_EOT:
					{
						cout << packet.getSBody() <<endl;
						return;
					}
					default:
					{
						Error::msg("unknown statid: %d", packet.getStatid());
						break;
					}
				}
				break;
			}
			case TAG_DATA:
			{
				switch(packet.getDataid())
				{
					// case DATA_FILE:
					// {
					// 	cout << "DATA_FILE" <<endl;
					// 	break;
					// }
					case DATA_TEXT:
					{
						cout << packet.getSBody();
						break;
					}
					default:
					{
						Error::msg("unknown statid: %d", packet.getStatid());
						break;
					}
				}
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

void CliPI::cmdQUIT(std::vector<string> & paramVector)
{
	if(paramVector.size() != 0)
	{
		std::cout << "Usage: " << helpMap["QUIT"] << std::endl;
		return;
	}

	Socket::tcpClose(connfd);

	exit(1);
}

void CliPI::cmdHELP(std::vector<string> & paramVector)
{
    if(paramVector.size() == 0)
	{
		int i = 1;
		std::cout << "commands:" << std::endl;
		for (map<string ,string>::iterator iter=helpMap.begin(); iter!=helpMap.end(); ++iter, ++i)
	    {
	            std::cout << "\t" << iter->first;
	            if (i % 5 == 0)
	            {
	            	std::cout << std::endl;
	            }
	    }

	    if ((i -1) % 5 != 0){
	    	std::cout << std::endl;
	    }
	    
	} else if(paramVector.size() == 1){
		map<string, string>::iterator iter = helpMap.find(toUpper(paramVector[0]));
		if (iter != helpMap.end())
		{
			std::cout << "Usage: " << helpMap[toUpper(paramVector[0])] << std::endl;
		} else {
	        std::cerr << paramVector[0] << ": command not found"  << std::endl;
		}
	} else {
		std::cout << "Usage: " << helpMap["HELP"] << std::endl;
	}                                                                  
	return;
}

int CliPI::getFileNslice(const char *pathname, uint32_t *pnslice_o)  
{  
 
    unsigned long filesize = 0, n = MAXNSLICE;

    struct stat statbuff;  
    if(stat(pathname, &statbuff) < 0){  
        return -1;  // error
    } else {  
        if (statbuff.st_size == 0)
		{
			return 0; // file is empty.
		} else {
			filesize = statbuff.st_size;  
		}  
    }  
    if (filesize % SLICECAP == 0)
	{
		 *pnslice_o = filesize/SLICECAP; 
	} else if ( (n = filesize/SLICECAP + 1) > MAXNSLICE ){
		Error::msg("too large file size: %d\n (MAX: %d)", n, MAXNSLICE);
		return -2; 
	} else {
		 *pnslice_o = filesize/SLICECAP + 1; 
	}
  	//printf("getFileNslice nslice: %d\n", *pnslice_o);
    return 1;  
}

string CliPI::toUpper(string &s)
{
	string upperStr;
	for(string::size_type i=0; i < s.size(); i++)
		upperStr += toupper(s[i]);
	return upperStr;
}

string CliPI::toLower(string &s)
{
	string upperStr;
	for(string::size_type i=0; i < s.size(); i++)
		upperStr += tolower(s[i]);
	return upperStr;
}
int CliPI::getConnfd()
{
	return this->connfd;
}
void CliPI::saveUserState()
{
	std::cout<< "\n\033[32msave user state ok\033[0m" << std::endl;
}