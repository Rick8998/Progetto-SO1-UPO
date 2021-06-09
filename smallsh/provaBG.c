#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	printf("\nINIZIO PROVA \n");
	sleep(atoi(argv[1]));
	printf("\nFINE PROVA \n");
}