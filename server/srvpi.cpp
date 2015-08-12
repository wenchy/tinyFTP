#include    "srvpi.h"

void SrvPI::run(int connfd)
{
	connSockStream.init(connfd);

	packet.reset(NPACKET);
	if ( connSockStream.Readn(packet.ps, PACKSIZE) == 0)
	            Error::quit_pthread("client terminated prematurely");
    packet.ntohp();
    //packet.print();
    if (packet.ps->tagid == TAG_CMD)
    {
    	switch(packet.ps->cmdid)
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
    int nend=0;   
    int nbegin=0;   
    while(nend != -1)   
    {   
        nend = src.find_first_of(token, nbegin);   
        if(nend == -1)   
            vect.push_back(src.substr(nbegin, src.length()-nbegin));   
        else  
            vect.push_back(src.substr(nbegin, nend-nbegin));   
        nbegin = nend + 1;   
    }   
}

static void Split(const char* content, const char* token, vector<std::string>& vect)   
{   
    if(content == NULL)   
           return;   
    int len = strlen(content);   
    if(len  <= 0)   
        return;   
    char* pBuf =(char*)malloc(len+1);   
    strcpy(pBuf , content); 
    // not thread safe  
    char* str = strtok(pBuf , token);   
    while(str != NULL)   
    {   
        vect.push_back(str);   
        str = strtok(NULL, token);   
    }   
    free(pBuf);   
}
void SrvPI::cmdPASS()
{
	printf("PASS request\n");
	packet.ps->body[packet.ps->bsize] = 0;
	//string params =
	vector<string> paramVector; 
	split(packet.ps->body, "\t", paramVector);

	// for (vector<string>::iterator iter=paramVector.begin(); iter!=paramVector.end(); ++iter)
 //   	{
 //    	std::cout << *iter << '\n';
 //   	}

   	std::map<string, string> selectParamMap = {  {"username", paramVector[0]}, {"password", paramVector[1]} };
   	if (db.select("user", selectParamMap))
   	{
   		vector< map<string ,string> > resultMapVector = db.getResult();
   		if (!resultMapVector.empty())
   		{
   			// send STAT_OK
			packet.sendSTAT_OK(connSockStream, "Welcome! " + paramVector[0]);
   		} else {
   			// send STAT_ERR Response
			packet.sendSTAT_ERR(connSockStream, "error: username mismatch password");
   		}
   	} else {
   		// send STAT_ERR Response
		packet.sendSTAT_ERR(connSockStream, "Database select error");
   	}
}

void SrvPI::cmdGET()
{
	printf("GET request\n");

	srvDTP.init(connSockStream);
	packet.ps->body[packet.ps->bsize] = 0;
	srvDTP.sendFile(packet.ps->body);
}
void SrvPI::cmdPUT()
{
	printf("PUT request\n");
	
	srvDTP.init(connSockStream);
	packet.ps->body[packet.ps->bsize] = 0;
	srvDTP.recvFile(packet.ps->body);
}
void SrvPI::cmdLS()
{
	printf("LS request\n");
	char buf[MAXLINE];
	char body[PBODYCAP];
	string sbody;
	DIR* dir;

	if (packet.ps->bsize == 0)
	{
		dir= opendir(".");
	} else {
		packet.ps->body[packet.ps->bsize] = 0;
		dir= opendir(packet.ps->body);
	}
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
	while( (e = readdir(dir)) )
	{	
		if (e->d_type == 4)
		{
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

		if ( cnt !=0 && (cnt % 4) == 0)
		{
			snprintf(buf, MAXLINE, "%s\n", buf);
		}

		if ( (sbody.size() + strlen(buf)) > SLICECAP)
		{
			strcpy(body, sbody.c_str());
			packet.sendDATA(connSockStream, 0, 0, 0, strlen(body), body);
			sbody.clear();
		}
		sbody += buf;
		
	}

	strcpy(body, sbody.c_str());
	packet.sendDATA(connSockStream, 0, 0, 0, strlen(body), body);
	
	packet.sendSTAT_EOT(connSockStream);

}
void SrvPI::cmdCD()
{
	printf("CD request\n");

	char buf[MAXLINE] = {0};
	int n;
	packet.ps->body[packet.ps->bsize] = 0;
	if( (n = chdir(packet.ps->body)) == -1)
	{
		// send STAT_ERR Response
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		// send STAT_OK
		snprintf(buf, MAXLINE, "server: change CWD to [%s]", packet.ps->body);
		packet.sendSTAT_OK(connSockStream, buf);
	}
	//packet.sendSTAT_EOT(connSockStream);
}

void SrvPI::cmdRM()
{
	printf("RM request\n");

	char buf[MAXLINE];
	packet.ps->body[packet.ps->bsize] = 0;
	if( remove(packet.ps->body) !=0 )
	{
		// send STAT_ERR Response 
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		// send STAT_OK
		snprintf(buf, MAXLINE, "%s is removed", packet.ps->body);
		packet.sendSTAT_OK(connSockStream, buf);
	}
}

void SrvPI::cmdPWD()
{
	printf("PWD request\n");

	char buf[MAXLINE];
	if( !getcwd(buf, MAXLINE))
	{
		// send STAT_ERR Response
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		// send STAT_OK
		packet.sendSTAT_OK(connSockStream, buf);
	}
}

void SrvPI::cmdMKDIR()
{
	printf("MKDIR request\n");
	packet.ps->body[packet.ps->bsize] = 0;
	char buf[MAXLINE];
	DIR* d = opendir(packet.ps->body);
	if(d)
	{	
		packet.sendSTAT_ERR(connSockStream, "already exists");
		closedir(d);
	}
	else if(mkdir(packet.ps->body, 0777) == -1)
	{
		//fprintf(stderr, "Wrong path.\n");
		// send STAT_ERR Response
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		packet.sendSTAT_ERR(connSockStream, strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		// send STAT_OK
		snprintf(buf, MAXLINE, "directory %s is created", packet.ps->body);
		packet.sendSTAT_OK(connSockStream, buf);
	}
}