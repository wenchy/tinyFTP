#include 	"error.h"

/* Init the daemon_proc flag */
bool Error::daemon_proc = false;

/* Nonfatal error related to system call
 *  * Print message and return */
void Error::ret(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	doit(1, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error related to system call
 * Print message and terminate */
void Error::sys(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

/* Fatal error related to system call
 * Print message, dump core, and terminate */
void Error::dump(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	abort();		/* dump core and terminate */
	exit(1);		/* shouldn't get here */
}

/* Nonfatal error unrelated to system call
 * Print message and return */
void Error::msg(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	doit(0, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error unrelated to system call
 * Print message and terminate */
void Error::quit(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

/* Fatal error unrelated to system call
 * Print message and terminate */
void Error::quit_pthread(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	pthread_exit((void *)1); ;
}

/* Print message and return to caller
 * Caller specifies "errnoflag" and "level" */
void Error::doit(int errnoflag, int level, const char *fmt, va_list ap)
{
	int		errno_save, n;
	char	buf[MAXLINE + 1];
	char	errmsg[MAXLINE + 1];

	errno_save = errno;		/* value caller might want printed */
#ifdef	HAVE_VSNPRINTF
	vsnprintf(buf, MAXLINE, fmt, ap);	/* safe */
#else
	vsprintf(buf, fmt, ap);					/* not safe */
#endif
	n = strlen(buf);
	if (errnoflag)
	{
		if (strerror_r(errno_save, errmsg, MAXLINE) != 0)
		{
		 	snprintf(errmsg, MAXLINE, "strerror_r call failed");
		} 
		snprintf(buf + n, MAXLINE - n, ": %s", errmsg);
	}
	strcat(buf, "\n");

	if (Error::daemon_proc) {
		syslog(level, "%s", buf);
		return;
	} else {
		fflush(stdout);		/* in case stdout and stderr are the same */
		fputs(buf, stderr);
		fflush(stderr);
	}
	return;
}