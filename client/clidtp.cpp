#include    "clidtp.h"

CliDTP::CliDTP(Packet * ppacket, CliPI * pcliPI)
{ 
	this->ppacket = ppacket;
	this->pcliPI = pcliPI;
}

void CliDTP::sendFile(const char *pathname, FILE *fp, uint32_t nslice, uint32_t sindex, uint16_t slicecap)
{
	Packet & packet = *(this->ppacket);

	off64_t curpos = sindex * slicecap;
	if (lseek64(fileno(fp), curpos, SEEK_SET) < 0)
	{
		Error::ret("lseek64");
		return;
	}

	int n;
	char body[PBODYCAP];
	//int oldProgress = 0, newProgress = 0;
	string hfilesize =  getFileSizeString(pathname);
	if(nslice == 0)
	{
		fprintf(stderr, "\033[2K\r\033[0m%-40s%10s\t100%%", pathname, hfilesize.c_str());
	} else {
		while( (n = fread(body, sizeof(char), PBODYCAP, fp)) >0 )
		{
			packet.sendDATA_FILE(nslice, ++sindex, n, body);

			// newProgress = (sindex*1.0)/nslice*100;
			// if (newProgress > oldProgress)
			// {
			// 	fprintf(stderr, "\033[2K\r\033[0m%-40s%10s\t%3d%%", pathname, hfilesize.c_str(), newProgress);
			// }
			// oldProgress = newProgress;
		}

	}

	fclose(fp);
	//printf("EOF [%s]\n", pathname);
	packet.sendSTAT_EOF();

	// continue reading progress tip sended by server
	while (pcliPI->recvOnePacket())
	{
		if (packet.getTagid() == TAG_STAT && packet.getStatid() == STAT_PGS) 
		{
			cerr << packet.getSBody();
		} else if (packet.getTagid() == TAG_STAT && packet.getStatid() == STAT_WAIT){
			cerr << endl << packet.getSBody();
		} else if (packet.getTagid() == TAG_STAT && packet.getStatid() == STAT_EOT){
			//cout << endl << packet.getSBody() << endl;
			cout << endl;
			break;
		} else {
			cout << "unknown packet" << endl;
			packet.print();
			break;
		}
	}
}

void CliDTP::removeFile(const char *pathname)
{
	if( remove(pathname) !=0 )
	{
		Error::ret("remove");
		return;
	}
}
void CliDTP::recvFile(const char *pathname, FILE *fp)
{
	Packet & packet = *(this->ppacket);
	string hfilesize;
	int m;
	int oldProgress = 0, newProgress = 0;

	while(pcliPI->recvOnePacket())
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
						//cout << packet.getSBody() <<endl;
						break;
					}
					case STAT_SIZE:
					{
						//cout << packet.getSBody() <<endl;
						if (std::stoull(packet.getSBody()) > getDiskAvailable())
						{
							packet.sendSTAT_ERR("insufficient disk space");
							Error::msg("insufficient disk space");
							return;
						} else {
							packet.sendSTAT_OK("sufficient disk space, ok to tranfer");
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
						fclose(fp);
						//std::cout << "\n" << packet.getSBody() << std::endl;
						break;
					}
					case STAT_EOT:
					{
						//std::cout << packet.getSBody() << std::endl;
						std::cout << std::endl;
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
						m = fwrite(packet.getBody(), sizeof(char), packet.getBsize(), fp);

						if (m != packet.getBsize())
						{
							Error::msg("fwirte error %d/%d: %d vs %d Bytes\n", packet.getSindex(), packet.getNslice(), packet.getBsize(), m);
							fclose(fp);
							return;
						} else 
						{
							if(packet.getNslice() == 0)
							{
								fprintf(stderr, "\033[2K\r\033[0m%-40s%10s\t100%%", pathname, hfilesize.c_str());
								break;
							}
							newProgress = (packet.getSindex()*1.0)/packet.getNslice()*100;
							if (newProgress > oldProgress)
							{
								//printf("\033[2K\r\033[0m");
								fprintf(stderr, "\033[2K\r\033[0m%-40s%10s\t%3d%%", pathname, hfilesize.c_str(), newProgress);
							}
							oldProgress = newProgress;
						}
						//printf("Recieved packet %d: %d vs %d Bytes\n", packet.ps->sindex, packet.getBsize(), m);
						break;
					}
					case DATA_TEXT:
					{
						hfilesize =  packet.getSBody();
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

	// first receive response
	/*recvOnePacket();
	if (packet.getTagid() == TAG_STAT) {
		if (packet.getStatid() == STAT_OK) {
			//cout << "OK to transfer " << packet.getSBody() <<endl;
			hfilesize =  packet.getSBody();
		} else if (packet.getStatid() == STAT_ERR){
			cerr << packet.getSBody() <<endl;
			removeFile(pathname);
			return;
		} else {
			Error::msg("CliDTP::recvFile: unknown statid %d", packet.getStatid());
			removeFile(pathname);
			return;
		}
		
	} else {
		Error::msg("CliDTP::recvFile: unknown tagid %d", packet.getTagid());
		removeFile(pathname);
		return;
	}

	// second transfer file
	//fprintf(stdout, "Recieve file now: %s\n", pathname);

	int m;
	int oldProgress = 0, newProgress = 0;
	while (1)
	{
		recvOnePacket();
		if(packet.getTagid() == TAG_DATA) {
			m = fwrite(packet.getBody(), sizeof(char), packet.getBsize(), fp);

			if (m != packet.getBsize())
			{
				Error::msg("fwirte error %d/%d: %d vs %d Bytes\n", packet.getSindex(), packet.getNslice(), packet.getBsize(), m);
				fclose(fp);
				return;
			} else {
				if(packet.getNslice() == 0)
				{
					Error::msg("nslice is zero, can not divide\n");
					break;
				}
				newProgress = (packet.getSindex()*1.0)/packet.getNslice()*100;
				if (newProgress > oldProgress)
				{
					//printf("\033[2K\r\033[0m");
					fprintf(stderr, "\033[2K\r\033[0m%-30s%20s\t%3d%%", pathname, hfilesize.c_str(), newProgress);
				}
				oldProgress = newProgress;
			}
			//printf("Recieved packet %d: %d vs %d Bytes\n", packet.ps->sindex, packet.getBsize(), m);
		} else if(packet.getTagid() == TAG_STAT) {
			if (packet.getStatid() == STAT_EOF)
			{
				fclose(fp);
				//std::cout << "\n" << packet.getSBody() << std::endl;
				continue;
			} else if (packet.getStatid() == STAT_EOT){
				//std::cout << packet.getSBody() << std::endl;
				std::cout << std::endl;
				return;
			} else {
				Error::msg("SrvDTP::recvFile TAG_STAT: unknown statid %d", packet.getStatid());
				return;
			}
		} else {
			Error::msg("CliDTP::recvFile: unknown tagid %hu with statid %hu", packet.getTagid(), packet.getStatid());
			fclose(fp);
			return;
		}
	}*/
}

// int CliDTP::getFileNslice(const char *pathname,uint32_t *pnslice_o)  
// {  
 
//     unsigned long filesize = 0, n = MAXNSLICE;

//     struct stat statbuff;  
//     if(stat(pathname, &statbuff) < 0){  
//         return -1;  // error
//     } else {  
//         if (statbuff.st_size == 0)
// 		{
// 			return 0; // file is empty.
// 		} else {
// 			filesize = statbuff.st_size;  
// 		}  
//     }  
//     if (filesize % SLICECAP == 0)
// 	{
// 		 *pnslice_o = filesize/SLICECAP; 
// 	} else if ( (n = filesize/SLICECAP + 1) > MAXNSLICE ){
// 		Error::msg("too large file size: %d\n (MAX: %d)", n, MAXNSLICE);
// 		return -2; 
// 	} else {
// 		 *pnslice_o = filesize/SLICECAP + 1; 
// 	}
  
//     return 1;  
// }


string CliDTP::getFileSizeString(const char *pathname)  
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