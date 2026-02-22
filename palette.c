#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "palette.h"

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

	int *keys = NULL;
	int size = 0;

	while (word = strtok_r(ptr, " ", &save_ptr))
	{
		size++;
		keys = realloc(keys, size * sizeof *keys);

		errno = 0;
		keys[size - 1] = strtol(word, NULL, 10);

		if (errno != 0)
		{
			perror("strtol()");
			free(keys);
			return NULL;
		}

		ptr = NULL;
	}

	keys = realloc(keys, (size + 1) * sizeof *keys);
	keys[size] = -1;

	return keys;
}

int **parse_palette(const char *path)
{
	char *data = read_file(path);
	if (data == NULL)
	{
		return NULL;
	}

	char *ptr = data;
	char *line;
	char *save_ptr;

	int **palette = NULL;
	int num = 0;

	while (line = strtok_r(ptr, "\n", &save_ptr))
	{
		num++;
		palette = realloc(palette, num * sizeof *palette);

		if ((palette[num - 1] = parse_line(line)) == NULL)
		{
			free_palette(palette);
			free(data);
			return NULL;
		}

		ptr = NULL;
	}

	palette = realloc(palette, (num + 1) * sizeof *palette);
	palette[num] = NULL;

	return palette;
}

void free_palette(int **chords)
{
	for (int **ptr = chords; *ptr; ptr++)
	{
		free(*ptr);
	}
	free(chords);
}
