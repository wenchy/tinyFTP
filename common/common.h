/* common header.  Tabs are set for 4 spaces, not 8 */

#ifndef _TINYFTP_COMMON_H_
#define _TINYFTP_COMMON_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>     // sockaddr_in{} and other Internet definitions
#include <arpa/inet.h>      // inet(3) functions
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define DEBUG 1

// MACRO constants
#define	LISTENQ		1024		// 2nd argument(backlog) to listen()

// Miscellaneous constants
#define	MAXLINE		4096		// max text line length
#define	BUFFSIZE	8192		// 8K Bytes buffer size for reads and writes

#define PORT		2121		// server: listening port
#define CTRPORT		PORT    	// server: control port 
#define DATPORT		(PORT + 1)	// server: data port

// Following shortens all the typecasts of pointer arguments
#define	SA	struct sockaddr

typedef enum sockType
{
	SRV_SOCKET,					// server socket tyoe
	CLI_SOCKET					// client socket type
} SockType;

/*********************************************************
 ******************* packet ******************************
 *********************************************************/

#define HEADSIZE		12			// packet header size
#define	BODYCAP			512			// packet body capcacity, basic file slice size

// TCP packet definition, communication protocal
 #pragma pack(4)
typedef struct packet
{
	/* packet header */
	uint16_t sesid;			// Session id
	uint16_t type;			// Packet Type

	uint16_t cmdid;			// Command ID

	uint16_t nslice;		// Data: whole number of file slices
	uint16_t sindex;		// Data: slice index
	uint16_t bsize;			// Data: the real size of body 

	/* packet body */
	char body[BODYCAP];	// packet body
	
} Packet;

#define PACKSIZE sizeof(Packet)

// implicit communication conventions 
typedef enum packetType
{
	REQU,
	DONE,
	INFO,
	TERM,
	DATA,
	EOT			// End Of Transmission
}PacketType;

// different file transfer control commands
typedef enum cmdID
{
	USE = 1,
	PASS,
	GET,
	PUT,
	MGET,
	MPUT,
	DELE,
	LDELE,
	RGET,
	RPUT,
	MGETWILD,
	MPUTWILD,
	CD,
	LCD,
	LS,
	LLS,
	MKD,
	LMKD,
	PWD,
	LPWD,
	RMDIR,
	LRMDIR,
	BINARY,
	ASCII,
	EXIT
} CmdID;

typedef enum packetStoreType
{
	HPACKET,					// Host storage type
	NPACKET						// Network storage type
} PacketStoreType;


/*********************************************************
 ******************* functions ***************************
 *********************************************************/

void Fclose(FILE *fp);
FILE * Fdopen(int fd, const char *type);
char * Fgets(char *ptr, int n, FILE *stream);
FILE * Fopen(const char *filename, const char *mode);
void Fputs(const char *ptr, FILE *stream);

#endif	/* __TINYFTP_COMMON_H__ */
