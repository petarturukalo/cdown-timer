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

struct time {
	int secs;
	int mins;
	int hrs;
};

/*
 * Split seconds into formatted parts seconds, mins and hours to give
 * time in format hh:mm:ss.
 */
void seconds_to_fmttime(long seconds, struct time *t)
{
	t->secs = seconds%SECS_IN_MIN;
	t->mins = seconds/SECS_IN_MIN;
	t->hrs = t->mins/MINS_IN_HR;
	t->mins %= MINS_IN_HR;
}

/*
 * Convert formatted time hh:mm:ss into seconds.
 */
long fmttime_to_seconds(struct time *t)
{
	return t->secs + SECS_IN_MIN*t->mins + (SECS_IN_MIN*MINS_IN_HR*t->hrs);
}

/*
 * Print seconds in format hh:mm:ss.
 */
void print_time(long seconds)
{
	struct time t;

	seconds_to_fmttime(seconds, &t);

	printf("\r%02d:%02d:%02d", t.hrs, t.mins, t.secs);
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
	struct time t;

	// Secs, mins, and hours is 3 args. Add 1 for progam name.
	if (argc != 4) {
		fprintf(stderr, "Error: expected 3 integer args hours, mins, seconds.\n");
		exit(EXIT_FAILURE);
	}

	t.hrs = atoi(argv[1]);
	t.mins = atoi(argv[2]);
	t.secs = atoi(argv[3]);

	seconds = fmttime_to_seconds(&t);
	signal(SIGINT, handle_interrupt);

	// Start the timer and wait for it to finish.
	for (; seconds >= 0; --seconds) {
		print_time(seconds);
		sleep(1);
	}
	printf("\n");
	return 0;
}
