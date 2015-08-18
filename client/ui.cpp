#include    "ui.h"

// command init
map<const string, const uint16_t> UI::cmdMap = {    {"USER",    USER},
                                                    {"PASS",    PASS},

                                                    {"GET",     GET},
                                                    {"PUT",     PUT},
                                                    {"LS",      LS},
                                                    {"LLS",     LLS},
                                                    {"CD",      CD},
                                                    {"LCD",     LCD},
                                                    {"RM",      RM},
                                                    {"LRM",     LRM},
                                                    {"PWD",     PWD},
                                                    {"LPWD",    LPWD},
                                                    {"MKDIR",   MKDIR},
                                                    {"LMKDIR",  LMKDIR},
                                                    {"QUIT",    QUIT},
                                                    {"HELP",    HELP},

                                                    {"MGET",    MGET},
                                                    {"MPUT",    MPUT},
                                                    {"RGET",    RGET},
                                                    {"RPUT",    RPUT},
                                                    {"RMDIR",   RMDIR},

                                                    {"SHELL",   SHELL},
                                                    {"LSHELL",  LSHELL},

                                                    {"BINARY",  BINARY},
                                                    {"ASCII",   ASCII}
                                                                        };

UI::UI(const char *host): cliPI(host)
{  

}

void UI::run()
{ 
	string word;
	string inputline;

    // user validate commands
    //while (printf("\033[35mUsername for 'tinyFTP': \033[0m"), getline(std::cin, inputline))
    while (printf("Username for 'tinyFTP': "), getline(std::cin, inputline))
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


    int         maxfdp1;
    fd_set      rset;
    int connfd = cliPI.getConnfd();

    FD_ZERO(&rset);

    while(1) 
    {   printf("%s@tinyFTP> ", username.c_str());
        fflush(stdout);
        FD_SET(connfd, &rset);
        FD_SET(fileno(stdin), &rset);
        maxfdp1 = connfd + 1;
        if (select(maxfdp1, &rset, NULL, NULL, NULL) < 0)
            Error::sys("select error");

        if (FD_ISSET(connfd, &rset)) {  /* socket is readable */
            cliPI.recvOnePacket();
        }

        if (FD_ISSET(fileno(stdin), &rset)) /* input is readable */
        {  
            getline(std::cin, inputline);
            cmdRun(inputline);
        }
    }

	// other ftp commands: first cout prompt (use "," operator)
	// while (printf("%s@tinyFTP> ", username.c_str()), getline(std::cin, inputline))
	// {
	// 	cmdRun(inputline);
	// }                                                         
	
}
void UI::cmdRun(string & inputline)
{
    // clear cmdVector each time when user input
    this->cmdVector.clear();
    //std::cout << "inputline: " << inputline << inputline.size() << std::endl;

    // split input string
    for (auto it = inputline.begin(); it < inputline.end(); ++it)
    {
        string param;
        for(; it < inputline.end(); ++it)
        {
            if ((*it) == ' ' || (*it) == '\t')
            {
                break;
            } else if ((*it) == '\\' && (it + 1) != inputline.end() && *(it + 1) == ' ')
            {
                param += ' ';
                ++it;
            } else {
                param += *it;
            }
        } 
        if (!param.empty())
        {
            this->cmdVector.push_back(param);
        }
    }

    // for (auto it = cmdVector.cbegin(); it != cmdVector.cend(); ++it)
    //    std::cout << it->size() << "cmdVector: " << *it << std::endl;

    // std::istringstream is(inputline);
    // while(is >> word)
    //  this->cmdVector.push_back(word);

    if (!cmdCheck())
    {
        return;
    } else {
        // remove command word, others are params
        cmdVector.erase(cmdVector.begin());
        cliPI.run(this->cmdid, this->cmdVector);
    }

    // for (auto it = cmdVector.cbegin(); it != cmdVector.cend(); ++it)
    //     std::cout << "cmdVector" << *it << std::endl;
    // for (std::vector<string>::size_type i = 0; i < cmdVector.size(); i++)
    //     std::cout << cmdVecto
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
        // process escape character '\ '
        // for (auto it = cmdVector.begin() + 1; it != cmdVector.end(); ++it)
        // {
        //     if (it->back() == '\\' && (it + 1) != cmdVector.end())
        //     {
        //        it->pop_back();
        //        (*it) += " ";
        //        (*it) += *(it+1);
        //        cmdVector.erase(it+1);
        //        it = cmdVector.begin();
        //     }
        //    std::cout << "cmdVector: " << *it << std::endl;
        // }

       	//std::cout << "CommandID: " << iter->first << "(" << iter->second << ")" << std::endl;
        return true;
	} else {
        std::cerr << cmdVector[0] << ": command not found"  << std::endl;
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