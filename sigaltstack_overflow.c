/* t_sigaltstack_fin.c
   test overflow of the alternate signal stack
*/

#define _GNU_SOURCE             
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/wait.h> 
#include <err.h> 

#include "../kselftest.h" 

void 
printWaitStatus(const char *msg, int status)
 {
 	if (WIFEXITED(status)){
 	    ksft_print_msg("?FAIL\n");
 	    ksft_print_msg("child exited status=%d\n", WEXITSTATUS(status));
 	}
 	else if (WIFSIGNALED(status)){
 	    ksft_print_msg("[RUN]\t Overflowed the alternate signal stack\n");
 	    ksft_print_msg("[OK]\t Child killed by signal %d (%s) \n", WTERMSIG(status), strsignal(WTERMSIG(status)));
 	}   
 	if (WCOREDUMP(status))
 	    ksft_test_result_pass("core dumped\n");
 }
 
/* A recursive function that overflows the standart stack */
static void             
overflowStack(int callNum)
{ 
    char a[100000]; 
    overflowStack(callNum+1);
}

static void
sigsegvHandler(int sig)
{     
    ksft_print_msg("[RUN]\t Overflowed the standart signal stack\n");
    ksft_print_msg("[RUN]\t Caught signal %d (%s) at the alternate signal stack\n", sig, strsignal(sig));

    if (!raise(SIGFPE))
        ksft_test_result_skip("[NOTE]\tCan't send signal SIGFPE\n");      
    sigsegvHandler(SIGFPE);
}

int
main(int argc, char *argv[])
{
    stack_t sigstack;
    struct sigaction sa;
    pid_t childPid;
    int status;
    int err;
    
    ksft_print_header();
    ksft_set_plan(1);
   
    switch (fork()) {
    
    case -1:
        ksft_exit_fail_msg("fork() - %s\n", strerror(errno));
    	exit(EXIT_FAILURE);
        
    case 0: /*child*/

    /* Allocate alternate stack and inform kernel of its existence */
    printf("PID = %d\n", getpid());
    
    sigstack.ss_sp = malloc(3000);
    if (sigstack.ss_sp == NULL) {
    	ksft_exit_fail_msg("malloc() - %s\n", strerror(errno));
    	exit(EXIT_FAILURE);
    }     
    sigstack.ss_size = 3000; 
    sigstack.ss_flags = 0;
    err = sigaltstack(&sigstack, NULL);
    if (err) {
    	ksft_exit_fail_msg("sigaltstack() - %s\n", strerror(errno));
    	exit(EXIT_FAILURE);    
    }

    /* Establish handler for SIGSEGV */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_ONSTACK;    /* Handler uses alternate stack */
    sa.sa_handler = sigsegvHandler;        
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
    	ksft_exit_fail_msg("sigaction(SIGSEGV) - %s\n", strerror(errno));
    	exit(EXIT_FAILURE);
    }
    if (sigaction(SIGFPE, &sa, NULL) == -1) {
    	ksft_exit_fail_msg("sigaction(SIGFPE) - %s\n", strerror(errno));
    	exit(EXIT_FAILURE);
    }
    
    overflowStack(1);
    
    default: /*parent*/
    for (;;) {
        childPid = waitpid(-1, &status, WUNTRACED);      
	if (childPid == -1) {
            ksft_exit_fail_msg("waitpid() - %s\n", strerror(errno));
    	    exit(EXIT_FAILURE);
        }
        printWaitStatus(NULL, status);       
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            ksft_exit_pass();
            exit(EXIT_SUCCESS);
        }
 }
}
}


