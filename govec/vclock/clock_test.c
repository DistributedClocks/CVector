
#include "vclock.h"


int main(int argc, char *argv[]) {
	
	struct vectorClock * test = new(getppid());
	tick(test, getppid());
	tick(test, 22);
	tick(test, 22);
	tick(test, 22);
	tick(test, 22);
	tick(test, 44);
	printVC(test);
	printVC(test);

/*	struct vectorClock * fest = copy(test);
	printf("TEST\n");
	printVC(fest);*/
	free(test);
}


