#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "logging.h"

#ifndef SIGNALS_H
#define SIGNALS_H

void sig_int(int);
void sig_usr1(int);
void sig_usr2(int);

pid_t get_child_pgid();

pid_t child_pgid;
int STOP_WORK = 0;
int nr_child_processes = 0;

// Will return 1 if a signal handler couldn't be placed.
int setup_signal_handlers()
{
	// Get current process group.
	pid_t father_pgid = getpgrp();
	// Make a new process group for the child processes.
	child_pgid = father_pgid + 1;
	// Setup signal handlers.
	if (signal(SIGINT, sig_int) == SIG_ERR)	// Prevents CTRL+C normal behaviour.
		return 1; 
	if (signal(SIGUSR1, sig_usr1) == SIG_ERR)	// A signal received from the father process that makes the child temporarily stop its work.
		return 1; 
	if (signal(SIGUSR2, sig_usr2) == SIG_ERR)	// A signal received from the father process that makes the child resume its work.
		return 1; 

	return 0;
}

void sig_int(int signo)
{
	log_action("SINAL", "RECEBIDO INT");
	// Make all child processes stop!
	killpg(get_child_pgid(), SIGUSR1);
	log_action("SINAL", "USR1 enviado para filhos.");

	while(1)	// Ask user for input until he gives a proper answer.
	{
		printf("Are you sure you want to terminate the program ? (Y/N) ");
		char answer;
		scanf("%c", &answer);
		if(answer == 'Y' || answer == 'y')
		{
			killpg(get_child_pgid(), SIGKILL); // Kill all child processes
			log_action("SINAL", "KILL enviado para filhos.");
			return;
		}
		else if(answer == 'N' || answer == 'n')
		{
			killpg(get_child_pgid(), SIGUSR2); // Resume work on child processes
			log_action("SINAL", "USR2 enviado para filhos.");
			return;
		}
		else
		{
			printf("Not a valid answer!\n");
			continue;
		}
	}
}

void sig_usr1(int signo)
{
	log_action("SINAL", "RECEBIDO USR1");
	STOP_WORK = 1;
	while(STOP_WORK) { }
}

void sig_usr2(int signo)
{
	log_action("SINAL", "RECEBIDO USR2");
	STOP_WORK = 0;
}

pid_t get_child_pgid()
{
	return child_pgid;
}

void reset_number_of_childs()
{
	nr_child_processes = 0;
}

void new_child_was_created()
{
	nr_child_processes++;
}

void wait_for_child_processes()
{
	for (int id = 0; id < nr_child_processes; id++) {
		int status;
		wait(&status);
	}
}

#endif