#ifndef WATCHER_H
#define WATCHER_H

int write_pid_file(char* path);
void set_sigprocmask(sigset_t & sigset);
int watcher();

#endif // WATCHER_H
