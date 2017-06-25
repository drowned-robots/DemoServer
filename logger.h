#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>

void init_log();
void log(int msg_stat, const char * msg_format, ...);

#endif // LOGGER_H
