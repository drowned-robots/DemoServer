#ifndef TRANSACT_HANDLER_H
#define TRANSACT_HANDLER_H

#define CH_RUN     10
#define CH_KILL    20

#define FD_LIMIT 48*1024

int transact_handle();
int set_fd_limit(int max_count);
static void error_handle(int in_signal, siginfo_t *signal_info, void *pcontext);

#endif // TRANSACT_HANDLER_H
