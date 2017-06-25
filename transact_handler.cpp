#include <signal.h>
#include <string.h>
#include "execinfo.h"
#include "stdlib.h"

#include <stdio.h>
#include <unistd.h>
#include "errno.h"
#include <sys/resource.h>

#include "transact_handler.h"
#include "logger.h"


int set_fd_limit(int max_count)
{
    struct rlimit lim;
    lim.rlim_cur = max_count;
    lim.rlim_max = max_count;
    return setrlimit(RLIMIT_NOFILE, &lim);
}

static void error_handle(int in_signal, siginfo_t *signal_info, void *pcontext)
{
    void* err_addr;
    void* trace[16];
    int trace_lines;
    int trace_size;
    char** msgs;

    log(LOG_INFO, "%s: Signal: %s, Addr: 0x%0.16X", __PRETTY_FUNCTION__, strsignal(in_signal), signal_info->si_addr);

    #if __WORDSIZE == 64
        err_addr = (void*)((ucontext_t*)pcontext)->uc_mcontext.gregs[REG_RIP];
    #else
        ErrorAddr = (void*)((ucontext_t*)ptr)->uc_mcontext.gregs[REG_EIP];
    #endif

    trace_size = backtrace(trace, 16);
    trace[1] = err_addr;

    msgs = backtrace_symbols(trace, trace_size);
    if (msgs)
    {
        log(LOG_INFO, "%s: backtrace:", __PRETTY_FUNCTION__);
        for (trace_lines = 1; trace_lines < trace_size; trace_lines++)
            log(LOG_INFO, "%s", msgs[trace_lines]);
        free(msgs);
    }
    log(LOG_INFO, "%s. Deamon was stopped", __PRETTY_FUNCTION__);
    //DestroyWorkThread();
    exit(CH_RUN);
}

int transact_handle()
{
    struct sigaction sigact;
    sigset_t  sigset;
    int signo;
    int status;

    sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = error_handle;

    sigemptyset(&sigact.sa_mask);

    sigaction(SIGFPE, &sigact, 0); // ошибка FPU
    sigaction(SIGILL, &sigact, 0); // ошибочная инструкция
    sigaction(SIGSEGV, &sigact, 0); // ошибка доступа к памяти
    sigaction(SIGBUS, &sigact, 0); // ошибка шины при обращении к физической памяти

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);

    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    set_fd_limit(FD_LIMIT);

    log(LOG_INFO, "%s. Daemon is started", __PRETTY_FUNCTION__);

    status = 0;//InitWorkThread();
    if (!status)
    {
        while(true)
        {
            sigwait(&sigset, &signo);
            if (signo == SIGUSR1)
            {
                status = 0;//ReloadConfig();
                if (status)
                    log(LOG_INFO, "%s. Reload config failed", __PRETTY_FUNCTION__);
                else
                    log(LOG_INFO, "%s. Reload config OK", __PRETTY_FUNCTION__);

            }
            else
                break;
        }
        //DestroyWorkThread();
    }
    else
        log(LOG_INFO, "%s. Create work thread failed", __PRETTY_FUNCTION__);


    log(LOG_INFO,"%s. Daemon is stopped", __PRETTY_FUNCTION__);
    return CH_KILL;
}
