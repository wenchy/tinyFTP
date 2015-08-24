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
			cout << packet.getSBody() << endl;
			//cout << endl;
			break;
		} else {
			cout << "unknown packet" << endl;
			packet.print();
			break;
		}
	}
}

void CliDTP::recvFile(const char *pathname, FILE *fp, uint32_t nslice, uint32_t sindex, uint16_t slicecap)
{
	Packet & packet = *(this->ppacket);
	string hfilesize;
	int m;
	int oldProgress = 0, newProgress = 0;
	off64_t curpos = sindex * slicecap;
	if (lseek64(fileno(fp), curpos, SEEK_SET) < 0)
	{
		Error::ret("lseek64");
		return;
	}

	while(pcliPI->recvOnePacket())
	{
		switch(packet.getTagid())
		{
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
						//std::cerr << "\n" << packet.getSBody();
						fclose(fp);

						std::cout << std::endl;
						//std::cout << "\n" << packet.getSBody() << std::endl;
						break;
					}
					case STAT_EOT:
					{
						//std::cout << packet.getSBody() << std::endl;
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

						break;
					}
					case DATA_TEXT:
					{
						//cout << packet.getSBody() <<endl;
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
}


void CliDTP::removeFile(const char *pathname)
{
	if( remove(pathname) !=0 )
	{
		Error::ret("remove");
		return;
	}
}

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