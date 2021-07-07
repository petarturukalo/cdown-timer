/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define SECS_IN_MIN 60
#define MINS_IN_HR 60

/*
 * Split seconds into formatted parts seconds, mins and hours to give
 * time in format hh:mm:ss.
 */
void seconds_to_fmttime(long seconds, int *secs, int *mins, int *hrs)
{
	*secs = seconds%SECS_IN_MIN;
	*mins = seconds/SECS_IN_MIN;
	*hrs= *mins/MINS_IN_HR;
	*mins %= MINS_IN_HR;
}

/*
 * Convert formatted time hh:mm:ss into seconds.
 */
long fmttime_to_seconds(int secs, int mins, int hrs)
{
	return secs+SECS_IN_MIN*mins+(SECS_IN_MIN*MINS_IN_HR*hrs);
}

/*
 * Print seconds in format hh:mm:ss.
 */
void print_time(long seconds)
{
	int secs, mins, hrs;

	seconds_to_fmttime(seconds, &secs, &mins, &hrs);

	printf("\r%02d:%02d:%02d", hrs, mins, secs);
	fflush(stdout);
}

/*
 * Pause the program until the user enters to either
 * quit or resume.
 */
void handle_interrupt(int n)
{
	char c;

	printf("Enter 'q' to quit or 'r' to resume.\n");
	
	while ((c = getchar()) != 'r') {
		if (c == 'q')
			// Exit with failure so wrapping shell scripts don't continue 
			// a chained && (AND) operation.
			exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	long seconds;
	int secs, mins, hrs;

	// Secs, mins, and hours is 3 args. Add 1 for progam name.
	if (argc != 4) {
		fprintf(stderr, "Error: expected 3 integer args hours, mins, seconds.\n");
		exit(EXIT_FAILURE);
	}

	hrs = atoi(argv[1]);
	mins = atoi(argv[2]);
	secs = atoi(argv[3]);

	seconds = fmttime_to_seconds(secs, mins, hrs);
	signal(SIGINT, handle_interrupt);

	// Start the timer and wait for it to finish.
	for (; seconds >= 0; --seconds) {
		print_time(seconds);
		sleep(1);
	}

	printf("\n");
	
	return 0;
}
