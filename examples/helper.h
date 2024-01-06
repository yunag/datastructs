#ifndef YU_EXAMPLES_H
#define YU_EXAMPLES_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static int get_int(const char *message, bool *ok) {
  int id = 0;

  *ok = true;

  char buffer[20];
  fgets(buffer, sizeof(buffer), stdin);
  if (sscanf(buffer, "%d", &id) == 0) {
    *ok = false;
    puts(message);
  }

  return id;
}

static void get_string(char *dest, size_t dest_size) {
  fgets(dest, dest_size, stdin);
  dest[strcspn(dest, "\r\n")] = '\0';
}

#endif /* !YU_EXAMPLES_H */
