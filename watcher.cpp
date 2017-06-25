#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "errno.h"
#include <string.h>
#include <sys/resource.h>

#include "logger.h"
#include "transact_handler.h"
#include "watcher.h"


#define PID_FILE "/var/run/DemoServer.pid"

int write_pid_file(char* path)
{
    FILE * f;
    if (!(f = fopen(path, "w+")))
        return -1;
    fprintf(f, "%u", getpid());
    fclose(f);
    return 0;
}

void set_sigprocmask(sigset_t & sigset)
{
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGQUIT);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGCHLD);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, nullptr);
}

int watcher()
{
    int pid;
    int cur_status = 0;
    int * status = &cur_status;
    int need_start = 1;

    sigset_t sigset;
    siginfo_t siginfo;
    set_sigprocmask(sigset);

    write_pid_file(PID_FILE);

    while(true)
    {
        if (need_start)        
            pid = fork();        

        need_start = 0;

        if (pid == -1)
        {
          log(LOG_INFO, "%s: fork failed. %s", __PRETTY_FUNCTION__, strerror(errno));
        }
        else
            if (!pid)
            {
                *status = transact_handle();
                exit(*status);
            }

        sigwaitinfo(&sigset, &siginfo);

        if (siginfo.si_signo == SIGCHLD)
        {
            wait(status);
            cur_status = WEXITSTATUS(status);
            if (cur_status == CH_KILL)
            {
                log(LOG_INFO, "%s: child is stopped", __PRETTY_FUNCTION__);
                break;
            }
            if (cur_status == CH_RUN)
            {
                log(LOG_INFO, "%s: child is restarted", __PRETTY_FUNCTION__);
            }
        }
        else
        {
            log(LOG_INFO, "%s: signal. %s", __PRETTY_FUNCTION__, strsignal(siginfo.si_signo));
            kill(pid, SIGTERM);
            *status = 0;
            break;
        }
    }
    log(LOG_INFO, "%s. watcher is stopped", __PRETTY_FUNCTION__);
    unlink(PID_FILE);
    return *status;
}
