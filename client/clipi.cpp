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

                                            //{"BINARY",  "binary"},
                                            //{"ASCII",   "ascii"}  	
                                        								};

CliPI::CliPI(const char *host)
{
    Socket cliSocket(CLI_SOCKET, host, CTRPORT);
    connfd = cliSocket.init();
    connSockStream.init(connfd);
}

// void CliPI::init(const char *host)
// {
// 	int connfd;

//     Socket cliSocket(CLI_SOCKET, host, CTRPORT);
//     connfd = cliSocket.init();
//     connSockStream.init(connfd);

// }
// void CliPI::sendOnePacket()
// {
	
// }

bool CliPI::recvOnePacket()
{
	int n;
	packet.reset(NPACKET);
	if ( (n = connSockStream.Readn(packet.getPs(), PACKSIZE)) == 0)
	{
		Socket::tcpClose(connfd);
		Error::quit("server terminated prematurely");
	} else if (n < 0){
		Error::ret("connSockStream.Readn()");
		Error::quit("socket connection exception");
	}
	packet.ntohp();
	//packet.print();
	return true;
}

void CliPI::run(uint16_t cmdid, std::vector<string> & cmdVector)
{
	switch(cmdid)
	{
		case USER:
			cmdUSER(cmdVector);
			break;
		case PASS:
			cmdPASS(cmdVector);
			break;
		case GET:
			cmdGET(cmdVector);
			break;
		case RGET:
			cmdRGET(cmdVector);
			break;
		case PUT:
			cmdPUT(cmdVector);
			break;
		case LS:
			cmdLS(cmdVector);
			break;
		case LLS:
			cmdLLS(cmdVector);
			break;
		case CD:
			cmdCD(cmdVector);
			break;
		case LCD:
			cmdLCD(cmdVector);
			break;
		case RM:
			cmdRM(cmdVector);
			break;
		case LRM:
			cmdLRM(cmdVector);
			break;	
		case PWD:
			cmdPWD(cmdVector);
			break;
		case LPWD:
			cmdLPWD(cmdVector);
			break;
		case MKDIR:
			cmdMKDIR(cmdVector);
			break;
		case LMKDIR:
			cmdLMKDIR(cmdVector);
			break;
		case RMDIR:
			cmdRMDIR(cmdVector);
			break;
		case QUIT:
			cmdQUIT(cmdVector);
			break;
		case HELP:
			cmdHELP(cmdVector);
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
void CliPI::cmd2pack(uint16_t cmdid, std::vector<string> & cmdVector)
{
	packet.reset(HPACKET);
	string params;
	if (cmdVector.size() > 1)
	{
		vector<string>::iterator iter=cmdVector.begin() + 1;
		params += *iter;
		for (++iter; iter!=cmdVector.end(); ++iter)
	   	{
	   		params += DELIMITER + *iter;
	   	}
	}

	// Error::msg("body: %s\n", body);
	packet.fillCmd(cmdid, params.size(), params.c_str());
	//packet.print();
	packet.htonp(); 
}

void CliPI::userpass2pack(uint16_t cmdid, std::vector<string> & cmdVector)
{
	packet.reset(HPACKET);
	string params;
	vector<string>::iterator iter=cmdVector.begin();
	params += *iter;
	for (++iter; iter!=cmdVector.end(); ++iter)
   	{
   		params += DELIMITER + *iter;
   	}

	// Error::msg("body: %s\n", body);
	packet.fillCmd(cmdid, params.size(), params.c_str());
	//packet.print();
	packet.htonp(); 
}

bool CliPI::cmdUSER(std::vector<string> & cmdVector)
{
	if(cmdVector.size() != 1)
	{
		Error::msg("Usage: [username]");
		return false;
	} else {
		userpass2pack(USER, cmdVector);
		connSockStream.Writen(packet.getPs(), PACKSIZE);
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

bool CliPI::cmdPASS(std::vector<string> & cmdVector)
{
	if(cmdVector.size() != 2)
	{
		Error::msg("Usage: [password]");
		for (vector<string>::iterator iter=cmdVector.begin(); iter!=cmdVector.end(); ++iter)
	   	{
	    	std::cout << *iter << '\n';
	   	}
		return false;
	}

	userpass2pack(PASS, cmdVector);
	connSockStream.Writen(packet.getPs(), PACKSIZE);

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

void CliPI::cmdGET(std::vector<string> & cmdVector)
{
	printf("GET request\n");

	if(cmdVector.size() < 2 || cmdVector.size() > 3)
	{
		std::cout << cmdVector.size() << "Usage: " << helpMap["GET"] << std::endl;
		return;
	}

	string pathname;
	char buf[MAXLINE];
	if (cmdVector.size() == 2){
		vector<string> pathVector; 
		split(cmdVector[1], "/", pathVector);
		pathname = pathVector.back();
	} else if (cmdVector.size() == 3){
		pathname = cmdVector[2];
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
		// command to packet
		cmd2pack(GET, cmdVector);
	    connSockStream.Writen(packet.getPs(), PACKSIZE);
	}

    // pathname exist on server? need test
    CliDTP cliDTP(this->connSockStream, &(this->packet), this->connfd);
    //cliDTP.init(connSockStream, packet);
	cliDTP.recvFile(pathname.c_str(), fp);
 
}


void CliPI::cmdGET(string srvpath, string clipath)
{
	//printf("GET request: srvpath:\n");
	cout << "GET request: " << "srvpath=" << srvpath << " clipath=" << clipath << endl;

	char buf[MAXLINE];
	
	FILE *fp;
	if ((access(clipath.c_str(), F_OK)) == 0) {
		snprintf(buf, MAXLINE, "File [%s] already exists, overwrite ? (y/n) ", clipath.c_str());
		if(!confirmYN(buf))
		{
			packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
			return;
		}
	}

	if ( (fp = fopen(clipath.c_str(), "wb")) == NULL) {
		Error::msg("%s", strerror_r(errno, buf, MAXLINE));
		packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		packet.sendCMD_GET(connSockStream, srvpath);
	}

    CliDTP cliDTP(this->connSockStream, &(this->packet), this->connfd);
    //cliDTP.init(connSockStream, packet);
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

void CliPI::cmdRGET(std::vector<string> & cmdVector)
{
	if(cmdVector.size() != 2)
	{
		std::cout << "Usage: " << helpMap["RGET"] << std::endl;
		return;
	}
	char pathname[MAXLINE];
	char buf[MAXLINE];
	strcpy(pathname, cmdVector[1].c_str()); 

	if ((access(cmdVector[1].c_str(), F_OK)) == 0) { // already exists
		snprintf(buf, MAXLINE, "[%s] already exists, overwrite ? (y/n) ", pathname);
		if(!confirmYN(buf))
		{
			return;
		} else {
			// yes to overwite
			//removeDir(cmdVector[1].c_str(), false);
	   		string shellCMD = "rm -rf " + cmdVector[1];
			if (system(shellCMD.c_str()) == -1) {
				char buf[MAXLINE];
				printf("%s\n", strerror_r(errno, buf, MAXLINE));
				return;
			} else {
				// OK
				printf("Dir '%s' removed\n",  cmdVector[1].c_str());
			}
		}
	}

	if (system(("mkdir " + cmdVector[1]).c_str()) == -1) {
		char buf[MAXLINE];
		printf("%s\n", strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		// OK
		printf("Dir '%s' created\n",  cmdVector[1].c_str());
	}

	cmd2pack(RGET, cmdVector);
	connSockStream.Writen(packet.getPs(), PACKSIZE);

	
	CliDTP cliDTP(this->connSockStream, &(this->packet), this->connfd);

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
						cmdLMKDIR(packet.getSBody());
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
						cout << packet.getSBody() <<endl;
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
			default:
			{
				Error::msg("unknown tagid: %d", packet.getTagid());
				break;
			}
		}
	}

	// FILE *fp;

	// if ( (fp = fopen(pathname, "wb")) == NULL) {
	// 	Error::msg("%s", strerror_r(errno, buf, MAXLINE));
	// 	return;
	// } else {
	// 	// command to packet
	// 	cmd2pack(0, GET, cmdVector);
	//     connSockStream.Writen(packet.getPs(), PACKSIZE);
	// }

 //    CliDTP cliDTP(this->connSockStream, &(this->packet), this->connfd);
	// cliDTP.recvFile(pathname, fp);
}

void CliPI::cmdPUT(std::vector<string> & cmdVector)
{
	if(cmdVector.size() < 2 || cmdVector.size() > 3)
	{
		std::cout << "Usage: " << helpMap["PUT"] << std::endl;
		return;
	}

	char pathname[MAXLINE];
	char buf[MAXLINE];
	uint32_t nslice = 0;

	strcpy(pathname,cmdVector[1].c_str()); 
	struct stat statBuf;
    int n = stat(cmdVector[1].c_str(), &statBuf);
    if(!n) // stat call success
	{	
		if (S_ISREG(statBuf.st_mode)){
			;
	    } else if (S_ISDIR(statBuf.st_mode)){
			cout << "put: cannot upload [" << cmdVector[1] << "]: Is a directory" << endl;
			return;
	    } else {
	    	cout << "put: [" << cmdVector[1] << "] not a regular file or directory" << endl;
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
		// command to packet
		cmd2pack(PUT, cmdVector);
	    connSockStream.Writen(packet.getPs(), PACKSIZE);
	}

	while (1)
	{
		recvOnePacket();
		if (packet.getTagid() == TAG_STAT) {
			if (packet.getStatid() == STAT_OK) {
				cout << packet.getSBody() <<endl;
				// must contain sesssion id
				CliDTP cliDTP(this->connSockStream, &(this->packet), this->connfd);
				cliDTP.sendFile(pathname, fp, nslice);
				break;
			} else if (packet.getStatid() == STAT_CFM) {
				if(confirmYN(packet.getSBody().c_str()))
				{
					packet.sendSTAT_CFM(connSockStream, "y");
					continue;
				} else {
					packet.sendSTAT_CFM(connSockStream, "n");
					return;
				}
			} else if (packet.getStatid() == STAT_ERR) {
				cerr << packet.getSBody() <<endl;
				return;
			} else {
				Error::msg("CliDTP::sendFile: unknown statid %d", packet.getStatid());
				packet.print();
				return;
			}
			
		} else {
			Error::msg("CliDTP::sendFile: unknown tagid %d", packet.getTagid());
			packet.print();
			return;
		}
	}

	printf("EOT %s\n", pathname);
	packet.sendSTAT_EOT(connSockStream);
	
}
void CliPI::cmdLS(std::vector<string> & cmdVector)
{
	if(cmdVector.size() > 2)
	{
		std::cout << "Usage: " << helpMap["LS"] << std::endl;
		return;
	}
	
	cmd2pack(LS, cmdVector);
	connSockStream.Writen(packet.getPs(), PACKSIZE);

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

void CliPI::cmdLLS(std::vector<string> & cmdVector)
{
	string shellCMD = "ls --color=auto";
	for (auto it = cmdVector.begin() + 1; it != cmdVector.end(); ++it){
       	//std::cout << *it << std::endl;
       	shellCMD += " " + *it;
	}
	if (system(shellCMD.c_str()) == -1) {
		char buf[MAXLINE];
		std::cout << "system(): " << strerror_r(errno, buf, MAXLINE) << std::endl;
	}
}

void CliPI::cmdCD(std::vector<string> & cmdVector)
{
	if(cmdVector.size() != 2)
	{
		std::cout << "Usage: " << helpMap["CD"] << std::endl;
		return;
	}

	cmd2pack(CD, cmdVector);
	connSockStream.Writen(packet.getPs(), PACKSIZE);

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

void CliPI::cmdLCD(std::vector<string> & cmdVector)
{
	if(cmdVector.size() != 2)
	{
		std::cout << "Usage: " << helpMap["LCD"] << std::endl;
		return;
	}

	int n;
	//char buf[MAXLINE];
	if( (n = chdir(cmdVector[1].c_str())) == -1)
	{
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		Error::ret("lcd");
		return;
	}
}

void CliPI::cmdRM(std::vector<string> & cmdVector)
{
	if(cmdVector.size() != 2)
	{
		std::cout << "Usage: " << helpMap["RM"] << std::endl;
		return;
	}

	cmd2pack(RM, cmdVector);
	connSockStream.Writen(packet.getPs(), PACKSIZE);

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

void CliPI::cmdLRM(std::vector<string> & cmdVector)
{
	string shellCMD = "rm";
	for (auto it = cmdVector.begin() + 1; it != cmdVector.end(); ++it){
       	//std::cout << *it << std::endl;
       	shellCMD += " " + *it;
	}
	if (system(shellCMD.c_str()) == -1) {
		char buf[MAXLINE];
		std::cout << "system(): " << strerror_r(errno, buf, MAXLINE) << std::endl;
	}
}

void CliPI::cmdPWD(std::vector<string> & cmdVector)
{
	if(cmdVector.size() > 2)
	{
		std::cout << "Usage: " << helpMap["PWD"] << std::endl;
		return;
	} else if (cmdVector.size() == 2 && cmdVector[1] != "-a")
	{
		std::cout << "Usage: " << helpMap["PWD"] << std::endl;
		return;
	}

	cmd2pack(PWD, cmdVector);
	connSockStream.Writen(packet.getPs(), PACKSIZE);

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

void CliPI::cmdLPWD(std::vector<string> & cmdVector)
{
	string shellCMD = "pwd";
	for (auto it = cmdVector.begin() + 1; it != cmdVector.end(); ++it){
       	//std::cout << *it << std::endl;
       	shellCMD += " " + *it;
	}
	if (system(shellCMD.c_str()) == -1) {
		char buf[MAXLINE];
		std::cout << "system(): " << strerror_r(errno, buf, MAXLINE) << std::endl;
	}
}

void CliPI::cmdMKDIR(std::vector<string> & cmdVector)
{
	printf("MKDIR request\n");
	if(cmdVector.size() != 2)
	{
		std::cout << "Usage: " << helpMap["MKDIR"] << std::endl;
		return;
	}

	cmd2pack(MKDIR, cmdVector);
	connSockStream.Writen(packet.getPs(), PACKSIZE);

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


void CliPI::cmdLMKDIR(string path)
{
	printf("LMKDIR request\n");

	char buf[MAXLINE]; 
	if(mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1){
		printf("\033[31mmkdir [%s] failed: %s\033[0m\n", path.c_str(), strerror_r(errno, buf, MAXLINE));
		// send STAT_ERR Response
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		//msg_o += "system call (mkdir): ";
		//msg_o += strerror_r(errno, buf, MAXLINE);
		//packet.sendSTAT_ERR(connSockStream, msg_o.c_str());
	}else {
		printf("Dir [%s] created\n", path.c_str());
		// send STAT_OK
		//packet.sendSTAT_OK(connSockStream, paramVector[0] + " created");
	}

}

void CliPI::cmdLMKDIR(std::vector<string> & cmdVector)
{
	printf("LMKDIR request\n");
	string shellCMD = "mkdir";
	for (auto it = cmdVector.begin() + 1; it != cmdVector.end(); ++it){
       	//std::cout << *it << std::endl;
       	shellCMD += " " + *it;
	}
	if (system(shellCMD.c_str()) == -1) {
		char buf[MAXLINE];
		std::cout << "system(): " << strerror_r(errno, buf, MAXLINE) << std::endl;
	}
}

void CliPI::cmdRMDIR(std::vector<string> & cmdVector)
{
	if(cmdVector.size() != 2)
	{
		std::cout << "Usage: " << helpMap["RMDIR"] << std::endl;
		return;
	}

	cmd2pack(RMDIR, cmdVector);
	connSockStream.Writen(packet.getPs(), PACKSIZE);

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

void CliPI::cmdQUIT(std::vector<string> & cmdVector)
{
	if(cmdVector.size() != 1)
	{
		std::cout << "Usage: " << helpMap["QUIT"] << std::endl;
		return;
	}

	Socket::tcpClose(connfd);

	exit(1);
}

void CliPI::cmdHELP(std::vector<string> & cmdVector)
{
    if(cmdVector.size() == 1)
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
	    
	} else if(cmdVector.size() == 2){
		map<string, string>::iterator iter = helpMap.find(toUpper(cmdVector[1]));
		if (iter != helpMap.end())
		{
			std::cout << "Usage: " << helpMap[toUpper(cmdVector[1])] << std::endl;
		} else {
	        std::cerr << cmdVector[1] << ": command not found"  << std::endl;
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