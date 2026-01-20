#include <stdio.h>

/* data for the chopstick monitor */
static int chopsticks[] = {2, 2, 2, 2, 2};	/* chopstick counts */
static int myright[] = {1, 2, 3, 4, 0};		/* right neighbor identity */
static int myleft[] = {4, 0, 1, 2, 3};		/* left neighbor identity */
static CONDITION waiting[5];			/* condition variables */

/* initialization for chopstick monitor */
void monitor mchopstk()
{
	/* demonstrates that this is being called during initialization */
	printf("Initializing chopsticks monitor...\n");
}

/* monitor entry to get both chopsticks */
void entry GetChopsticks(id)
int id;
{
	/* wait until both chopsticks available */
	if (chopsticks[id] != 2)
		Wait(&waiting[id]);

	/* decrement neighbors' chopstick counts */
	chopsticks[myright[id]]--;
	chopsticks[myleft[id]]--;
}

/* monitor entry to release the 2 chopsticks */
void entry PutChopsticks(id)
int id;
{
	/* increment neighbors' chopstick counts and wake 
		them up if they now have both chopsticks */
	if (++chopsticks[myright[id]] == 2)
		Signal(&waiting[myright[id]]);
		
	if (++chopsticks[myleft[id]] == 2)
		Signal(&waiting[myleft[id]]);
		
}

