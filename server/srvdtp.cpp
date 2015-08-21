#include    "srvdtp.h"

void * md5sumThreadFunc(void * arg)
{
	ThreadArg * ptarg = (ThreadArg *)arg;
	const char * pathname = ptarg->buf;
	Database db = *(ptarg->pdb);
	printf("************md5sumThreadFunc: %s\n", pathname);   
	string md5str = md5sum(pathname);
	string sizestr = getFilesize(string(pathname));
	cout << "md5sumThreadFunc # filepath: " << pathname << "md5str: " << md5str << "sizestr: " << sizestr << endl;
	
	if (!md5str.empty() && !sizestr.empty())
	{
		std::map<string, string> insertParamMap = {    {"MD5SUM", md5str},
	                                                   {"MD5RAND", "NULL"},
	                                                   {"ABSPATH", pathname},
	                                                   {"SIZE", sizestr} };
		if (db.insert("file", insertParamMap))
        {
			Error::msg("Success: insert new file MD5SUM");
        } else {
           Error::msg("\033[31mDatabase insert error\033[0m");
        }   
	}   
    return(NULL);
}

SrvDTP::SrvDTP(Packet * ppacket, SrvPI * psrvPI)
{ 
	this->ppacket = ppacket;
	this->psrvPI = psrvPI;
}

void SrvDTP::insertNewFileMD5SUM(const char * pathname, Database *pdb)
{ 
	string md5str = md5sum(pathname);
	string sizestr = getFilesize(string(pathname));
	cout << "md5sumThreadFunc # filepath: " << pathname << "md5str: " << md5str << "sizestr: " << sizestr << endl;
	
	if (!md5str.empty() && !sizestr.empty())
	{
		std::map<string, string> selectParamMap = {  {"MD5SUM", md5str} };
		if (pdb->select("file", selectParamMap))
		{
			vector< map<string ,string> > resultMapVector = pdb->getResult();
			if (resultMapVector.empty())
			{
				std::map<string, string> insertParamMap = {    {"MD5SUM", md5str},
	                                                   {"MD5RAND", "NULL"},
	                                                   {"ABSPATH", pathname},
	                                                   {"SIZE", sizestr} };
				if (pdb->insert("file", insertParamMap))
		        {
					Error::msg("Success: insert new file MD5SUM");
		        } else {
		           Error::msg("\033[31mDatabase insert error\033[0m");
		        }   
			} else {
				std::map<string, string> whereParamMap = { {"MD5SUM", md5sum(pathname)} };
	    		std::map<string, string> updateParamMap = { {"VALID", "1"} };

				if (pdb->update("file", whereParamMap, updateParamMap))
		        {
					printf("Success: update VALID=1\n");
		        } else {
		           Error::msg("\033[31mDatabase update error\033[0m");
		        }
			}
		} else {
			 Error::msg("\033[31mDatabase select error\033[0m");
		}


		
	}   
}
void SrvDTP::sendFile(const char *pathname)
{
	Packet & packet = *(this->ppacket);
	int n;
	uint32_t nslice =0, sindex = 0;

	char buf[MAXLINE];

	//if ( (fp = fopen(pathname, "rb")) == NULL)
	if ( psrvPI->setFp(fopen(pathname, "rb")) == NULL)
	{
		// send STAT_ERR Response
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
		return;
	} else if ( (n = getFileNslice(pathname, &nslice)) <= 0)  {
		if ( n == 0) {
			printf("EOF[%s]: 0 bytes\n", pathname);
			Fclose(&psrvPI->getFp());
			packet.sendSTAT_OK(getFileSizeString(pathname));
			packet.sendDATA_TEXT(getFileSizeString(pathname));
			packet.sendDATA_FILE(0, 0, 0, NULL);
			packet.sendSTAT_EOF("EOF: 0 bytes");
			return;
		} else if ( n == -2) {
			snprintf(buf, MAXLINE, "Too large file size");
			packet.sendSTAT_ERR(buf);
		} else {
			snprintf(buf, MAXLINE, "File stat error");
			packet.sendSTAT_ERR(buf);
		}
		return;
	} else {
		// send STAT_OK
		packet.sendSTAT_OK(getFileSizeString(pathname));
	}

	packet.sendDATA_TEXT(getFileSizeString(pathname));

	char body[PBODYCAP];
	printf("Send [%s] now\n", pathname);

	while( (n = fread(body, sizeof(char), PBODYCAP, psrvPI->getFp())) >0 )
	{
		packet.sendDATA_FILE(nslice, ++sindex, n, body);
	}
	
	// int			maxfdp1;
	// fd_set		rset, wset;
	// int connfd = psrvPI->getConnfd();

	// FD_ZERO(&rset);
	// while( (n = fread(body, sizeof(char), PBODYCAP, psrvPI->getFp())) >0 ) {
	// 	FD_SET(connfd, &rset);
	// 	FD_SET(connfd, &wset);
	// 	maxfdp1 = connfd + 1;
	// 	if (select(maxfdp1, &rset, &wset, NULL, NULL) < 0)
	// 		Error::sys("select error");

	// 	if (FD_ISSET(connfd, &rset)) {	/* socket is readable */
	// 		psrvPI->recvOnePacket();
	// 	}

	// 	if (FD_ISSET(connfd, &wset)) {  /* socket is writable */
	// 		packet.sendDATA_FILE(nslice, ++sindex, n, body);
	// 	}
	// }
	
	// send EOF
	Fclose(&psrvPI->getFp());
	printf("EOF [%s]\n", pathname);
	packet.sendSTAT_EOF();
}

ThreadArg threadArg;
pthread_t tid;
void SrvDTP::recvFile(const char *pathname)
{
	Packet & packet = *(this->ppacket);
	char buf[MAXLINE];

	if ( psrvPI->setFp(fopen(pathname, "wb")) == NULL)
	{
		// send STAT_ERR Response
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		// send STAT_OK
		packet.sendSTAT_OK();
	}
	
	printf("Recv file [%s] now\n", pathname);
	int m;

	while (psrvPI->recvOnePacket())
	{
		if(packet.getTagid() == TAG_DATA && packet.getDataid() == DATA_FILE){
			m = fwrite(packet.getBody(), sizeof(char), packet.getBsize(), psrvPI->getFp());
			if (m != packet.getBsize())
			{
				Error::msg("Recieved slice %d/%d: %d vs %d Bytes\n", packet.getSindex(), packet.getNslice(), packet.getBsize(), m);
				return;
			}
			//printf("Recieved packet %d: %d vs %d Bytes\n", packet.ps->sindex, packet.ps->bsize, m);
		} else if(packet.getTagid() == TAG_STAT) {
			if (packet.getStatid() == STAT_EOF)
			{
				Fclose(&psrvPI->getFp());
				std::cout << packet.getSBody() << std::endl;
				
				// threadArg.pdb = psrvPI->getPDB();
				// snprintf(threadArg.buf, MAXLINE, "%s", pathname);
				// Pthread_create(&tid, NULL, &md5sumThreadFunc, &threadArg);

				insertNewFileMD5SUM(pathname, psrvPI->getPDB());
				continue;
			} else if (packet.getStatid() == STAT_EOT){
				std::cout << packet.getSBody() << std::endl;
				return;
			} else {
				Error::msg("SrvDTP::recvFile TAG_STAT: unknown statid %d", packet.getStatid());
				return;
			}
			
		} else {
			Error::msg("SrvDTP::recvFile: unknown tagid %d with statid %d", packet.getTagid(), packet.getStatid());
			return;
		}
	}
}

void SrvDTP::recvFile(const char *pathname, uint32_t nslice, uint32_t sindex, uint16_t slicecap)
{
	Packet & packet = *(this->ppacket);
	char buf[MAXLINE];
	

	if ( psrvPI->setFp(fopen(pathname, "ab")) == NULL)
	{
		// send STAT_ERR Response
		// GNU-specific strerror_r: char *strerror_r(int errnum, char *buf, size_t buflen);
		packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
		return;
	} else {
		off64_t n;
		off64_t curpos = sindex * slicecap;
		if ( ( n = lseek64(fileno(psrvPI->getFp()), curpos, SEEK_SET)) < 0)
		{
			packet.sendSTAT_ERR(strerror_r(errno, buf, MAXLINE));
			return;
		} else {
			printf("Recv file [%s %u/%u] now\n", pathname, sindex, nslice);
			// send STAT_OK
			packet.sendSTAT_OK();
		}
	}
	
	int m;

	while (psrvPI->recvOnePacket())
	{
		if(packet.getTagid() == TAG_DATA && packet.getDataid() == DATA_FILE){
			m = fwrite(packet.getBody(), sizeof(char), packet.getBsize(), psrvPI->getFp());
			if (m != packet.getBsize())
			{
				Error::msg("Recieved slice %d/%d: %d vs %d Bytes\n", packet.getSindex(), packet.getNslice(), packet.getBsize(), m);
				return;
			}
			//printf("Recieved packet %d: %d vs %d Bytes\n", packet.ps->sindex, packet.ps->bsize, m);
		} else if(packet.getTagid() == TAG_STAT) {
			if (packet.getStatid() == STAT_EOF)
			{
				Fclose(&psrvPI->getFp());
				std::cout << packet.getSBody() << std::endl;

				// threadArg.pdb = psrvPI->getPDB();
				// snprintf(threadArg.buf, MAXLINE, "%s", pathname);
				// Pthread_create(&tid, NULL, &md5sumThreadFunc, &threadArg);
				insertNewFileMD5SUM(pathname, psrvPI->getPDB());

				continue;
			} else if (packet.getStatid() == STAT_EOT){
				std::cout << packet.getSBody() << std::endl;
				return;
			} else {
				Error::msg("SrvDTP::recvFile TAG_STAT: unknown statid %d", packet.getStatid());
				return;
			}
			
		} else {
			Error::msg("SrvDTP::recvFile: unknown tagid %d with statid %d", packet.getTagid(), packet.getStatid());
			return;
		}
	}
}

int SrvDTP::getFileNslice(const char *pathname,uint32_t *pnslice_o)  
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
  	//printf("getFileNslice nslice: %u\n", *pnslice_o);
    return 1;  
}

string SrvDTP::getFileSizeString(const char *pathname)  
{  
 
    unsigned long filesize = 0;
    unsigned long n = 0;
    string hsize_o;
    char buf[MAXLINE];
    unsigned long kbase = 1024;
    unsigned long mbase = 1024 * 1024;
    unsigned long gbase = 1024 * 1024 * 1024;


    struct stat statbuff;  
    if(stat(pathname, &statbuff) < 0){
    	hsize_o = "error"; 
        return hsize_o;  // error
    } else {  
        if (statbuff.st_size == 0)
		{
			hsize_o = "0B"; // file is empty.
		} else {
			filesize = statbuff.st_size;
			if (filesize / kbase == 0)
			{ 
				snprintf(buf, MAXLINE, "%lu", filesize);
				hsize_o += buf;
				hsize_o +="B";
			} else if ( filesize / mbase == 0 ){
				snprintf(buf, MAXLINE, "%lu", filesize / kbase);
				hsize_o += buf;
				n = (filesize % kbase)* 100 / kbase;
				if (n != 0)
				{
					hsize_o += ".";
					snprintf(buf, MAXLINE, "%02lu", n);
					hsize_o += buf;
				}
				hsize_o +="K";
			} else if ( filesize / gbase == 0 ){
				snprintf(buf, MAXLINE, "%2lu", filesize / mbase);
				hsize_o += buf;
				n = (filesize % mbase)* 100 / mbase;
				if (n != 0)
				{
					hsize_o += ".";
					snprintf(buf, MAXLINE, "%02lu", n);
					hsize_o += buf;
				}
				hsize_o +="M";
			} else {
				snprintf(buf, MAXLINE, "%lu", filesize / gbase);
				hsize_o += buf;
				n = (filesize % gbase) * 100 / gbase ;
				//printf("filesize n: %lu\n", n);
				if (n != 0)
				{
					hsize_o += ".";
					snprintf(buf, MAXLINE, "%02lu", n);
					hsize_o += buf;
				}
				hsize_o +="G";
			}
		}  
    }  
	return hsize_o;
}