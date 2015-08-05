#ifndef _TINYFTP_ERROR_H_
#define _TINYFTP_ERROR_H_

#include    "common.h"

#include    <stdarg.h>      /* ANSI C header file */
#include    <syslog.h>      /* for syslog() */


class Error
{
public:
    /* Nonfatal error related to system call
     *  * Print message and return */
    static void ret(const char *fmt, ...);

    /* Fatal error related to system call
     *  * Print message and terminate */
    static void sys(const char *fmt, ...);
    
    /* Fatal error related to system call
     *  * Print message, dump core, and terminate */
    static void dump(const char *fmt, ...);

    /* Nonfatal error unrelated to system call
     *  * Print message and return */
    static void msg(const char *fmt, ...);

    /* Fatal error unrelated to system call
     *  * Print message and terminate */
    static void quit(const char *fmt, ...);

private:
    static void doit(int, int, const char *, va_list);
    static bool daemon_proc;        /* set true by daemon_init() */
};

#endif /* _TINYFTP_ERROR_H_ */