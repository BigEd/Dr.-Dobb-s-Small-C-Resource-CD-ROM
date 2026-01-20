#include <stdio.h>

unsigned int seed = 127;

/* generate pseudo-random number between 1-200 inclusive */
rnd()
{

	seed *= 177;
	if (!seed)
		seed = 127;
	return seed % 200 + 1;
}

/* logic for a dining philosopher */
void philosopher(id)
int id;
{
	char buffer[128];
	int n;

	while (1)
	{
		/* acquire chopsticks */
		GetChopsticks(id);

		/* display message and eat */
		n = rnd();
        	sprintf(buffer, "Philosopher #%d eating for %d ticks :-)\n", 
			id, n);
		Send(buffer);	/* pass to console task */
		Delay(n);	/* simulate eating */

		/* release chopsticks */
		PutChopsticks(id);

		/* display message and think */
		n = rnd();
        	sprintf(buffer, "Philosopher #%d thinking for %d ticks :-(\n", 
			id, n);
		Send(buffer);	/* pass to console task */
		Delay(n);	/* simulate thinking */
	}
}

/* task declarations */
void task p0()
{
	philosopher(0);
}

void task p1()
{
	philosopher(1);
}

void task p2()
{
	philosopher(2);
}

void task p3()
{
	philosopher(3);
}

/* main program: note that the function philosopher is used as
	the body of a number of tasks including main() */
main()
{
	/* all declared tasks start before this. main() is converted
		into a task by Concurrent Small C and becomes the
		fifth philosopher */
	philosopher(4);

}

