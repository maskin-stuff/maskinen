#include <stdio.h>

#include "chords.h"

int main(int argc, char **argv)
{
	int **chords = parse_chords("palette.txt");

	for (int **chord_ptr = chords; *chord_ptr; chord_ptr++)
	{
		for (int *key_ptr = *chord_ptr; *key_ptr >= 0; key_ptr++)
		{
			printf(" %d", *key_ptr);
		}
		printf("\n");
	}
}
