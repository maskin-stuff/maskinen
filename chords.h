#ifndef CHORDS_H
#define CHORDS_H

int **parse_chords(const char *path);
void free_chords(int **chords);

extern const char *parse_chords_error;

#endif
