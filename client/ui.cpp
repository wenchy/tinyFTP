#include    "ui.h"

// command init
map<const string, const uint16_t> UI::cmdMap = {    {"USER",    USER},
                                                    {"PASS",    PASS},
                                                    {"GET",     GET},
                                                    {"PUT",     PUT},
                                                    {"MGET",    MGET},
                                                    {"MPUT",    MPUT},
                                                    {"DELE",    DELE},
                                                    {"RGET",    RGET},
                                                    {"RPUT",    RPUT},
                                                    {"CD",      CD},
                                                    {"LS",      LS},
                                                    {"MKD",     MKD},
                                                    {"RMD",     RMD},
                                                    {"BINARY",  BINARY},
                                                    {"ASCII",   ASCII},
                                                    {"QUIT",    QUIT},

                                                    {"INFO",    INFO},
                                                    {"EOT",     EOT}        };

UI::UI(int cliCtrConnfd, int cliDatConnfd): cliPI(cliCtrConnfd, cliDatConnfd)
{
	this->cliCtrConnfd = cliCtrConnfd;
	this->cliDatConnfd = cliDatConnfd;
}

void UI::run()
{ 
	string word;
	string inputline;

	// user interface: first cout prompt (use "," operator)
	while (std::cout << "tinyFTP> ", getline(std::cin, inputline))
	{
		// clear cmdVector each time when user input
		cmdVector.clear();
		//std::cout << inputline << std::endl;

		// split input string
		std::istringstream is(inputline);
		while(is >> word)
			cmdVector.push_back(word);

		if (!cmdCheck())
		{
			continue;
		} else {
			cliPI.cmd2pack(cmdid, cmdVector);
			cliPI.getCmd();
			//str_cli(cliCtrConnfd, cmdVector[0]);
		}

		// for (auto it = cmdVector.cbegin(); it != cmdVector.cend(); ++it)
  //   		std::cout << *it << std::endl;
    	// for (std::vector<string>::size_type i = 0; i < cmdVector.size(); i++)
    	// 	std::cout << cmdVector[i] << std::endl;
	}
	
	                                                     
	    
	
}

bool UI::cmdCheck()
{
	map<const string, const uint16_t >::iterator iter = cmdMap.find(toUpper(cmdVector[0]));
	if (iter != cmdMap.end())
	{
		this->cmdid = iter->second;
       	std::cout << "CommandID: " << iter->first << "(" << iter->second << ")" << std::endl;
        return true;
	} else {
        std::cerr << "unknown command: " << cmdVector[0] << std::endl;
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
