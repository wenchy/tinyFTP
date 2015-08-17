#ifndef _TINYFTP_UI_H_
#define _TINYFTP_UI_H_

#include    "../common/common.h"
#include    "../common/error.h"
#include 	"../common/sockstream.h"

#include    "clipi.h"


typedef struct command
{
	short int 	id;
	std::vector<string> argVec;

} Command;

// User Interface (UI)
class UI
{
public:
	UI(const char *host);
	void run();

private:
	// first elemment is command. other is options
	std::vector<string> cmdVector;
	uint16_t cmdid;
	CliPI cliPI;
	string username;

	// decode the input line to a command
	bool cmdCheck();
	//void appendArg(struct command* c, char* s);
	static map<const string, const uint16_t> cmdMap;

	string toUpper(string &s);
};

#endif /* _TINYFTP_UI_H_ */