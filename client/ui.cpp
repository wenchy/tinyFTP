#include    "ui.h"

// command init
map<const string, const uint16_t> UI::cmdMap = {    {"USER",    USER},
                                                    {"PASS",    PASS},

                                                    {"GET",     GET},
                                                    {"PUT",     PUT},
                                                    {"LS",      LS},
                                                    {"CD",      CD},
                                                    {"RM",      RM},
                                                    {"PWD",     PWD},
                                                    {"MKDIR",   MKDIR},

                                                    {"MGET",    MGET},
                                                    {"MPUT",    MPUT},
                                                    {"RGET",    RGET},
                                                    {"RPUT",    RPUT},
                                                    {"RMD",     RMD},

                                                    {"BINARY",  BINARY},
                                                    {"ASCII",   ASCII},
                                                    {"QUIT",    QUIT}
                                                                        };

UI::UI(const char *host)
{  
    cliPI.init(host);

}

void UI::run()
{ 
	string word;
	string inputline;

    // user validate commands
    while (printf("\033[35mUsername for 'tinyFTP': \033[0m"), getline(std::cin, inputline))
    {
        // clear cmdVector each time when user input
        this->cmdVector.clear();
        std::istringstream is(inputline);
        while(is >> word)
            this->cmdVector.push_back(word);

        // if user enter nothing, assume special anonymous user
        if (this->cmdVector.empty())
        {
            this->cmdVector.push_back("anonymous");
            this->cmdVector.push_back("anonymous"); 
            if (!cliPI.cmdPASS(this->cmdVector))
            {
                continue;
            } else {
                break;
            }
        }

        if (!cliPI.cmdUSER(this->cmdVector))
        {
            continue;
        } else {
            char *password = getpass("\033[35mPassword for 'tinyFTP': \033[0m");
            // printf("\033[35mPassword for 'tinyFTP': \033[0m");
            // getline(std::cin, inputline);
            // std::istringstream isPass(inputline);
            // while(isPass >> word)
            // {
            //      this->cmdVector.push_back(word);
            //      //std::cout << word << endl;
            // }
            this->cmdVector.push_back(password);  
            if (!cliPI.cmdPASS(this->cmdVector))
            {
                continue;
            } else {
                break;
            }
        }
    }                
    this->username = this->cmdVector[0];

	// other ftp commands: first cout prompt (use "," operator)
	while (printf("\033[35m%s@tinyFTP> \033[0m", username.c_str()), getline(std::cin, inputline))
	{
		// clear cmdVector each time when user input
		this->cmdVector.clear();
		//std::cout << inputline << std::endl;

		// split input string
		std::istringstream is(inputline);
		while(is >> word)
			this->cmdVector.push_back(word);

		if (!cmdCheck())
		{
			continue;
		} else {
			cliPI.run(this->cmdid, this->cmdVector);
		}

		// for (auto it = cmdVector.cbegin(); it != cmdVector.cend(); ++it)
        //    std::cout << *it << std::endl;
    	// for (std::vector<string>::size_type i = 0; i < cmdVector.size(); i++)
    	// 	   std::cout << cmdVector[i] << std::endl;
	}                                                         
	
}

bool UI::cmdCheck()
{
    if (cmdVector.empty())
    {
        return false;
    }
	map<const string, const uint16_t >::iterator iter = cmdMap.find(toUpper(cmdVector[0]));
	if (iter != cmdMap.end())
	{
		this->cmdid = iter->second;
       	//std::cout << "CommandID: " << iter->first << "(" << iter->second << ")" << std::endl;
        return true;
	} else {
        std::cerr << "UI#unknown command: " << cmdVector[0] << std::endl;
        return false;
	}
}

string UI::toUpper(string &s)
{
	string upperStr;
	for(string::size_type i=0; i < s.size(); i++)
		upperStr += toupper(s[i]);
	return upperStr;
}

void UI::str_cli(int sockfd, string msg)
{
    char  sendline[MAXLINE], recvline[MAXLINE];
    strcpy(sendline,msg.c_str());
    int n = strlen(sendline); 
    sendline[n+1] = '\0';
    sendline[n] = 10;
    std::cerr << "sendline: " << sendline << "size:" << strlen(sendline) << std::endl;
    SockStream connSockStream(sockfd);
    connSockStream.Writen(sendline, strlen(sendline));

    if (connSockStream.Readline(recvline, MAXLINE) == 0)
        Error::quit("str_cli: server terminated prematurely");

    Fputs(recvline, stdout);

}
