/* common header.  Tabs are set for 4 spaces, not 8 */

#ifndef _TINYFTP_COMMON_H_
#define _TINYFTP_COMMON_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>     // sockaddr_in{} and other Internet definitions
#include <arpa/inet.h>      // inet(3) functions
#include <netdb.h>
#include <pthread.h>
#include <signal.h>




#include <unistd.h>
#include <errno.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
// c++ header
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
using namespace std;

// using std::map;
// using std::string;
// using std::iostream;

#define DEBUG 1

// MACRO constants
#define	LISTENQ		1024		// 2nd argument(backlog) to listen()

// Miscellaneous constants
#define	MAXLINE		128			// max text line length
#define	BUFFSIZE	8192		// 8K Bytes buffer size for reads and writes

//#define PORT		2121		// server: listening port
#define CTRPORT		2121    	// server: control listening port
#define DATPORT		2020		// server: data pistening port

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
typedef enum packetStoreType
{
	HPACKET,					// Host storage type
	NPACKET						// Network storage type
} PacketStoreType;

/************************** CtrPacket **********************************/

#define PHEADSIZE		18			// packet header size
#define	PBODYCAP		512			// packet body capcacity
#define	SLICECAP		512			// slice capcacity
#define	MAXNSLICE		4294967295	// max slice count: the maximum positive integer value
// TCP control packet definition, communication protocol
 #pragma pack(4)
typedef struct packetStruct
{
	/* packet header */
	uint32_t sesid;			// Session id
	uint16_t tagid;			// different packet type: CMD, DATA, INFO 

	uint16_t cmdid;			// Command: ID
	uint16_t statid;		// status code id

	uint32_t nslice;		// Data: whole number of file slices
	uint32_t sindex;		// Data: slice index

	uint16_t bsize;			// the real size of body
	/* packet body */
	char body[PBODYCAP];	// packet body
	
} PacketStruct;

// // tagid: CMD
// #pragma pack(4)
// typedef struct cmdBodyStruct
// {
// 	uint16_t cmdid;			// Command: ID
	
// } CmdBodyStruct;

// // tagid: INFO
// #pragma pack(4)
// typedef struct infoBodyStruct
// {
// 	uint16_t status;			// status code
	
// } InfoBodyStruct;

// // tagid: Data
// #pragma pack(4)
// typedef struct dataBodyStruct
// {
// 	uint16_t nslice;		// Data: whole number of file slices
// 	uint16_t sindex;		// Data: slice index
// 	char slice[SLICECAP];	// packet body
	
// } DataBodyStruct;

#define PACKSIZE sizeof(PacketStruct)

typedef enum tagID
{
	TAG_CMD = 1,
	TAG_STAT,
	TAG_DATA
} TagID;

// different file transfer control commands
typedef enum cmdID
{
	USER = 1,
	PASS,
	GET,
	PUT,
	MGET,
	MPUT,
	DELE,
	RGET,
	RPUT,
	CD,
	LS,
	MKD,
	PWD,
	RMD,
	BINARY,
	ASCII,
	QUIT
} CmdID;

typedef enum statID
{
	STAT_OK = 1,
	STAT_ERR,
	STAT_TERM,
	STAT_DONE,
	STAT_EOT
} StatID;

/*********************************************************
 ******************* functions ***************************
 *********************************************************/

void Fclose(FILE *fp);
FILE * Fdopen(int fd, const char *type);
char * Fgets(char *ptr, int n, FILE *stream);
FILE * Fopen(const char *filename, const char *mode);
void Fputs(const char *ptr, FILE *stream);
void * Malloc(size_t size);



void	Pthread_create(pthread_t *, const pthread_attr_t *,
					   void * (*)(void *), void *);
void	Pthread_join(pthread_t, void **);
void	Pthread_detach(pthread_t);
void	Pthread_kill(pthread_t, int);

void	Pthread_mutexattr_init(pthread_mutexattr_t *);
void	Pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
void	Pthread_mutex_init(pthread_mutex_t *, pthread_mutexattr_t *);
void	Pthread_mutex_lock(pthread_mutex_t *);
void	Pthread_mutex_unlock(pthread_mutex_t *);

void	Pthread_cond_broadcast(pthread_cond_t *);
void	Pthread_cond_signal(pthread_cond_t *);
void	Pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
void	Pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *,
							   const struct timespec *);

void	Pthread_key_create(pthread_key_t *, void (*)(void *));
void	Pthread_setspecific(pthread_key_t, const void *);
void	Pthread_once(pthread_once_t *, void (*)(void));


#endif	/* __TINYFTP_COMMON_H__ */
