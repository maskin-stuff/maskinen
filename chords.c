#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "chords.h"

static char *read_file(const char *path)
{
	FILE *f = fopen(path, "r");
	if (f == NULL)
	{
		perror("fopen()");
		return NULL;
	}

	fseek(f, 0, SEEK_END);

	size_t size = ftell(f);

	fseek(f, 0, SEEK_SET);

	char *data = malloc(size + 1);

	size_t offset = 0;
	while (offset < size)
	{
		offset += fread(&data[offset], 1, size - offset, f);
	}

	fclose(f);
	data[size] = 0;

	return data;
}

static int *parse_line(char *line)
{
	char *ptr = line;
	char *word;
	char *save_ptr;

	int *chord = NULL;
	int size = 0;

	while (word = strtok_r(ptr, " ", &save_ptr))
	{
		size++;
		chord = realloc(chord, size * sizeof *chord);

		errno = 0;
		chord[size - 1] = strtol(word, NULL, 10);

		if (errno != 0)
		{
			perror("strtol()");
			free(chord);
			return NULL;
		}

		ptr = NULL;
	}

	chord = realloc(chord, (size + 1) * sizeof *chord);
	chord[size] = -1;

	return chord;
}

int **parse_chords(const char *path)
{
	char *data = read_file(path);
	if (data == NULL)
	{
		return NULL;
	}

	char *ptr = data;
	char *line;
	char *save_ptr;

	int **chords = NULL;
	int nchords = 0;

	while (line = strtok_r(ptr, "\n", &save_ptr))
	{
		nchords++;
		chords = realloc(chords, nchords * sizeof *chords);

		if ((chords[nchords - 1] = parse_line(line)) == NULL)
		{
			free_chords(chords);
			free(data);
			return NULL;
		}

		ptr = NULL;
	}

	chords = realloc(chords, (nchords + 1) * sizeof *chords);
	chords[nchords] = NULL;

	return chords;
}

void free_chords(int **chords)
{
	for (int **ptr = chords; *ptr; ptr++)
	{
		free(*ptr);
	}
	free(chords);
}
