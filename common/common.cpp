#include "common.h"
#include "error.h"
void
Fclose(FILE *fp)
{
	if (fclose(fp) != 0)
		Error::sys("fclose error");
}

void
Fclose(FILE **fp)
{
	if (fclose(*fp) != 0)
		Error::sys("fclose error");
	*fp = NULL;
}

FILE *
Fdopen(int fd, const char *type)
{
	FILE	*fp;

	if ( (fp = fdopen(fd, type)) == NULL)
		Error::sys("fdopen error");

	return(fp);
}

char *
Fgets(char *ptr, int n, FILE *stream)
{
	char	*rptr;

	if ( (rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
		Error::sys("fgets error");

	return (rptr);
}

FILE *
Fopen(const char *filename, const char *mode)
{
	FILE	*fp;

	if ( (fp = fopen(filename, mode)) == NULL)
		Error::sys("fopen error");

	return(fp);
}

void
Fputs(const char *ptr, FILE *stream)
{
	if (fputs(ptr, stream) == EOF)
		Error::sys("fputs error");
}
void *
Malloc(size_t size)
{
	void	*ptr;

	if ( (ptr = malloc(size)) == NULL)
		Error::sys("malloc error");
	memset(ptr, 0, size);
	return(ptr);
}


void
Pthread_create(pthread_t *tid, const pthread_attr_t *attr,
			   void * (*func)(void *), void *arg)
{
	int		n;

	if ( (n = pthread_create(tid, attr, func, arg)) == 0)
		return;
	errno = n;
	Error::sys("pthread_create error");
}

void
Pthread_join(pthread_t tid, void **status)
{
	int		n;

	if ( (n = pthread_join(tid, status)) == 0)
		return;
	errno = n;
	Error::sys("pthread_join error");
}

void
Pthread_detach(pthread_t tid)
{
	int		n;

	if ( (n = pthread_detach(tid)) == 0)
		return;
	errno = n;
	Error::sys("pthread_detach error");
}

void
Pthread_kill(pthread_t tid, int signo)
{
	int		n;

	if ( (n = pthread_kill(tid, signo)) == 0)
		return;
	errno = n;
	Error::sys("pthread_kill error");
}

void
Pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
	int		n;

	if ( (n = pthread_mutexattr_init(attr)) == 0)
		return;
	errno = n;
	Error::sys("pthread_mutexattr_init error");
}

#ifdef	_POSIX_THREAD_PROCESS_SHARED
void
Pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int flag)
{
	int		n;

	if ( (n = pthread_mutexattr_setpshared(attr, flag)) == 0)
		return;
	errno = n;
	Error::sys("pthread_mutexattr_setpshared error");
}
#endif

void
Pthread_mutex_init(pthread_mutex_t *mptr, pthread_mutexattr_t *attr)
{
	int		n;

	if ( (n = pthread_mutex_init(mptr, attr)) == 0)
		return;
	errno = n;
	Error::sys("pthread_mutex_init error");
}

/* include Pthread_mutex_lock */
void
Pthread_mutex_lock(pthread_mutex_t *mptr)
{
	int		n;

	if ( (n = pthread_mutex_lock(mptr)) == 0)
		return;
	errno = n;
	Error::sys("pthread_mutex_lock error");
}
/* end Pthread_mutex_lock */

void
Pthread_mutex_unlock(pthread_mutex_t *mptr)
{
	int		n;

	if ( (n = pthread_mutex_unlock(mptr)) == 0)
		return;
	errno = n;
	Error::sys("pthread_mutex_unlock error");
}

void
Pthread_cond_broadcast(pthread_cond_t *cptr)
{
	int		n;

	if ( (n = pthread_cond_broadcast(cptr)) == 0)
		return;
	errno = n;
	Error::sys("pthread_cond_broadcast error");
}

void
Pthread_cond_signal(pthread_cond_t *cptr)
{
	int		n;

	if ( (n = pthread_cond_signal(cptr)) == 0)
		return;
	errno = n;
	Error::sys("pthread_cond_signal error");
}

void
Pthread_cond_wait(pthread_cond_t *cptr, pthread_mutex_t *mptr)
{
	int		n;

	if ( (n = pthread_cond_wait(cptr, mptr)) == 0)
		return;
	errno = n;
	Error::sys("pthread_cond_wait error");
}

void
Pthread_cond_timedwait(pthread_cond_t *cptr, pthread_mutex_t *mptr,
					   const struct timespec *tsptr)
{
	int		n;

	if ( (n = pthread_cond_timedwait(cptr, mptr, tsptr)) == 0)
		return;
	errno = n;
	Error::sys("pthread_cond_timedwait error");
}

void
Pthread_once(pthread_once_t *ptr, void (*func)(void))
{
	int		n;

	if ( (n = pthread_once(ptr, func)) == 0)
		return;
	errno = n;
	Error::sys("pthread_once error");
}

void
Pthread_key_create(pthread_key_t *key, void (*func)(void *))
{
	int		n;

	if ( (n = pthread_key_create(key, func)) == 0)
		return;
	errno = n;
	Error::sys("pthread_key_create error");
}

void
Pthread_setspecific(pthread_key_t key, const void *value)
{
	int		n;

	if ( (n = pthread_setspecific(key, value)) == 0)
		return;
	errno = n;
	Error::sys("pthread_setspecific error");
}



int getFileNslice(const char *pathname, uint32_t *pnslice_o)  
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
  	//printf("getFileNslice nslice: %d\n", *pnslice_o);
    return 1;  
}

string getFileSizeString(const char *pathname)  
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

string visualmd5sum(const char * pathname)
{
    int n;
    char buf[SLICECAP];
    unsigned char out[MD5_DIGEST_LENGTH];
    string md5str;
    int oldProgress = 0, newProgress = 0;
    MD5_CTX ctx;

    uint32_t nslice = 0, sindex = 0;
    string 	tipstr;
    		tipstr += "\033[32mMD5SUM\033[0m(";
    		tipstr += pathname;
    		tipstr += ")";
    string hfilesize = getFileSizeString(pathname);
    if ( (n = getFileNslice(pathname, &nslice)) < 0) {
		Error::msg("getFileNslice error");
		return md5str;
	}
   
	FILE *fp;
    if ( (fp = fopen(pathname, "rb")) == NULL)
	{
		Error::ret("md5sum#fopen");
		return md5str;
	}

	MD5_Init(&ctx);
	while( (n = fread(buf, sizeof(char), SLICECAP, fp)) >0 )
	{
		MD5_Update(&ctx, buf, n);
		if (nslice > (1024 * 512))
		{
			newProgress = (++sindex*1.0)/nslice*100;
			if (newProgress > oldProgress)
			{
				//printf("\033[2K\r\033[0m");
				fprintf(stderr, "\033[2K\r\033[0m%-40s%10s\t%3d%%", tipstr.c_str(), hfilesize.c_str(), newProgress);
			}
			oldProgress = newProgress;
		}	
	}
	if (nslice > (1024 * 512))
		printf("\n");

    MD5_Final(out, &ctx);

    for(n = 0; n< MD5_DIGEST_LENGTH; n++)
	{
		snprintf(buf, SLICECAP, "%02x", out[n]);
		md5str += buf;
	}

    return md5str;        
}

string md5sum(const char * pathname)
{
    int n;
    char buf[SLICECAP];
    unsigned char out[MD5_DIGEST_LENGTH];
    string md5str;
    MD5_CTX ctx;

   
	FILE *fp;
    if ( (fp = fopen(pathname, "rb")) == NULL)
	{
		Error::ret("md5sum#fopen");
		return md5str;
	}

	MD5_Init(&ctx);
	while( (n = fread(buf, sizeof(char), SLICECAP, fp)) >0 )
	{
		MD5_Update(&ctx, buf, n);
	}
	printf("\n");

    MD5_Final(out, &ctx);

    for(n = 0; n< MD5_DIGEST_LENGTH; n++)
	{
		snprintf(buf, SLICECAP, "%02x", out[n]);
		md5str += buf;
	}

    return md5str;        
}

string visualmd5sumNslice(const char * pathname, uint32_t nslice)
{
    int n;
    char buf[SLICECAP];
    unsigned char out[MD5_DIGEST_LENGTH];
    string md5str;
    int oldProgress = 0, newProgress = 0;
    MD5_CTX ctx;

    uint32_t fileslice =0, sindex = 0;
    if ( (n = getFileNslice(pathname, &fileslice)) < 0) {
		Error::msg("getFileNslice error");
		return md5str;
	}
    int percent = (nslice*1.0)/fileslice*100;
	snprintf(buf, SLICECAP, "%u/%u  %3d%%",  nslice, fileslice, percent);
	string 	tipstr;
    		tipstr += "\033[32mMD5SUM\033[0m(";
    		tipstr += pathname;
    		tipstr += "  ";
    		tipstr += buf;
    		tipstr += ")";	
    string hfilesize = getFileSizeString(pathname);
     
   
	FILE *fp;
    if ( (fp = fopen(pathname, "rb")) == NULL)
	{
		Error::ret("md5sum#fopen");
		return md5str;
	}

	MD5_Init(&ctx);
	while( (n = fread(buf, sizeof(char), SLICECAP, fp)) >0 )
	{
		MD5_Update(&ctx, buf, n);
		if ((++sindex) == nslice)
		{
			break;
		}
		if (nslice > (1024 * 512))
		{
			newProgress = (sindex*1.0)/nslice*100;
			if (newProgress > oldProgress)
			{
				fprintf(stderr, "\033[2K\r\033[0m%-40s%10s\t%3d%%", tipstr.c_str(), hfilesize.c_str(), newProgress);
			}
			oldProgress = newProgress;
		}
		
	}
	if (nslice > (1024 * 512))
		printf("\n");

    MD5_Final(out, &ctx);

    for(n = 0; n< MD5_DIGEST_LENGTH; n++)
	{
		snprintf(buf, SLICECAP, "%02x", out[n]);
		md5str += buf;
	}

    return md5str;        
}

string md5sumNslice(const char * pathname, uint32_t nslice)
{
    int n;
    char buf[SLICECAP];
    unsigned char out[MD5_DIGEST_LENGTH];
    string md5str;
    MD5_CTX ctx;
    uint32_t sindex = 0;
   
	FILE *fp;
    if ( (fp = fopen(pathname, "rb")) == NULL)
	{
		Error::ret("md5sum#fopen");
		return md5str;
	}

	MD5_Init(&ctx);
	while( (n = fread(buf, sizeof(char), SLICECAP, fp)) >0 )
	{
		MD5_Update(&ctx, buf, n);
		if ((++sindex) == nslice)
		{
			break;
		}
	}

    MD5_Final(out, &ctx);

    for(n = 0; n< MD5_DIGEST_LENGTH; n++)
	{
		snprintf(buf, SLICECAP, "%02x", out[n]);
		md5str += buf;
	}

    return md5str;        
}

string md5sum(const char * str, int len)
{
	int n;
    MD5_CTX ctx;
    char buf[SLICECAP];
    unsigned char out[MD5_DIGEST_LENGTH];
    string md5str;

	MD5_Init(&ctx);
	MD5_Update(&ctx, str, len);
    MD5_Final(out, &ctx);

    for(n = 0; n< MD5_DIGEST_LENGTH; n++)
	{
		snprintf(buf, SLICECAP, "%02x", out[n]);
		md5str += buf;
	}

    return md5str;     
}

unsigned long long getFilesize(const char * pathname)
{
	struct stat statbuff;  
	if(stat(pathname, &statbuff) < 0)
	{
		Error::ret("getFilesize#stat");
		return 0;
	} else 
	{
		return (unsigned long long)statbuff.st_size;
	}
}

string getFilesize(string pathname)
{
	struct stat statbuff;  
	char buf[MAXLINE];
	string sizestr;
	if(stat(pathname.c_str(), &statbuff) < 0)
	{
		Error::ret("getFilesize#stat");
		return sizestr;
	} else 
	{
		snprintf(buf, MAXLINE, "%llu", (unsigned long long)statbuff.st_size);
		sizestr = buf;
		return sizestr;
	}
}

string size2str(unsigned long filesize)  
{  
    unsigned long n = 0;
    string hsize_o;
    char buf[MAXLINE];
    unsigned long kbase = 1024;
    unsigned long mbase = 1024 * 1024;
    unsigned long gbase = 1024 * 1024 * 1024;


 	if (filesize == 0)
	{
		hsize_o = "0B"; // file is empty.
	} 
	else 
	{
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
	return hsize_o;
}

// string getFileSizeString(const char *pathname)  
// {  
 
//     unsigned long filesize = 0;
//     unsigned long n = 0;
//     string hsize_o;
//     char buf[MAXLINE];
//     unsigned long kbase = 1024;
//     unsigned long mbase = 1024 * 1024;
//     unsigned long gbase = 1024 * 1024 * 1024;


//     struct stat statbuff;  
//     if(stat(pathname, &statbuff) < 0){
//     	hsize_o = "error"; 
//         return hsize_o;  // error
//     } else {  
//         if (statbuff.st_size == 0)
// 		{
// 			hsize_o = "0B"; // file is empty.
// 		} else {
// 			filesize = statbuff.st_size;
// 			if (filesize / kbase == 0)
// 			{ 
// 				snprintf(buf, MAXLINE, "%lu", filesize);
// 				hsize_o += buf;
// 				hsize_o +="B";
// 			} else if ( filesize / mbase == 0 ){
// 				snprintf(buf, MAXLINE, "%lu", filesize / kbase);
// 				hsize_o += buf;
// 				n = (filesize % kbase)* 100 / kbase;
// 				if (n != 0)
// 				{
// 					hsize_o += ".";
// 					snprintf(buf, MAXLINE, "%02lu", n);
// 					hsize_o += buf;
// 				}
// 				hsize_o +="K";
// 			} else if ( filesize / gbase == 0 ){
// 				snprintf(buf, MAXLINE, "%2lu", filesize / mbase);
// 				hsize_o += buf;
// 				n = (filesize % mbase)* 100 / mbase;
// 				if (n != 0)
// 				{
// 					hsize_o += ".";
// 					snprintf(buf, MAXLINE, "%02lu", n);
// 					hsize_o += buf;
// 				}
// 				hsize_o +="M";
// 			} else {
// 				snprintf(buf, MAXLINE, "%lu", filesize / gbase);
// 				hsize_o += buf;
// 				n = (filesize % gbase) * 100 / gbase ;
// 				//printf("filesize n: %lu\n", n);
// 				if (n != 0)
// 				{
// 					hsize_o += ".";
// 					snprintf(buf, MAXLINE, "%02lu", n);
// 					hsize_o += buf;
// 				}
// 				hsize_o +="G";
// 			}
// 		}  
//     }  
// 	return hsize_o;
// }



string encryptPassword(string password)
{
	string saltedPass = PASSSALT0 + password + PASSSALT1;
	//cout << "***********saltedPass: " << saltedPass << endl;
	saltedPass = md5sum(saltedPass.c_str(), saltedPass.size());
	//cout << "***********saltedPass: " << saltedPass << endl;
    return saltedPass;     
}



