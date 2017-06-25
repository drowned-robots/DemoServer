#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>

#include "logger.h"

#define LOG_PATH "./DemoServer_log"

void init_log()
{
    openlog("DemoServer", LOG_PID, LOG_LOCAL7);
}

void log(int msg_stat, const char * msg_format, ...)
{
    va_list args;
    va_start(args, msg_format);
    switch(msg_stat)
    {
    case LOG_CRIT:
        syslog(LOG_CRIT, msg_format, args);
       // exit(EXIT_FAILURE);
        break;
    case LOG_ERR:
        syslog(LOG_ERR, msg_format, args);
      //  exit(EXIT_FAILURE);
        break;
    default:
        syslog(LOG_INFO, msg_format, args);
        break;
    }
}


