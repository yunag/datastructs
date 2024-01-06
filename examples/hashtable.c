#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "datastructs/functions.h"
#include "datastructs/hash_table.h"

#include "helper.h"

struct user_info {
  int id;
  char name[31];

  struct hash_entry hh;
};

void add_user(hash_table *htable, int id, char *name) {
  struct user_info query;
  query.id = id;

  struct user_info *user = htable_find(htable, &query, hh);
  if (user != NULL) {
    /* User found */
    strcpy(user->name, name);
    return;
  }

  /* User not found */
  user = malloc(sizeof(*user));
  user->id = id;
  strcpy(user->name, name);

  htable_add(htable, user, hh);
}

struct user_info *find_user(hash_table *htable, int id) {
  struct user_info query;
  query.id = id;

  return htable_find(htable, &query, hh);
}

void delete_user(hash_table *htable, int id) {
  struct user_info query;
  query.id = id;

  struct user_info *user = htable_delete(htable, &query, hh);
  free(user);
}

void print_user(struct user_info *user) {
  printf("{ id: %d, name: %s }\n", user->id, user->name);
}

void print_users(hash_table *htable) {
  struct user_info *current;

  htable_for_each(htable, current, hh) { print_user(current); }
}

bool compare_by_id(const struct hash_entry *a, const struct hash_entry *b) {
  struct user_info *user1 = htable_entry(a, struct user_info, hh);
  struct user_info *user2 = htable_entry(b, struct user_info, hh);
  /* Sort in ascending order */
  return user1->id < user2->id;

  /* Sort in descending order */
  // return user1->id > user2->id;
}

bool compare_by_name(const struct hash_entry *a, const struct hash_entry *b) {
  struct user_info *user1 = htable_entry(a, struct user_info, hh);
  struct user_info *user2 = htable_entry(b, struct user_info, hh);
  /* Sort in ascending order */
  return strcmp(user1->name, user2->name) < 0;

  /* Sort in descending order */
  // return strcmp(user1->name, user2->name) > 0;
}

size_t hash_user(const struct hash_entry *entry) {
  struct user_info *user = htable_entry(entry, struct user_info, hh);
  /* You can implement hash function by yourself */
  return yu_hash_i32(user->id);
}

bool equal_user(const struct hash_entry *a, const struct hash_entry *b) {
  struct user_info *user1 = htable_entry(a, struct user_info, hh);
  struct user_info *user2 = htable_entry(b, struct user_info, hh);
  /* Compare against key */
  return user1->id = user2->id;
}

int main(int argc, char **argv) {
  hash_table *htable = htable_create(1, hash_user, equal_user);

  bool should_run = true;
  bool ok;

  struct user_info *user;

  int command, id;
  char name[sizeof(user->name)];

  while (should_run) {
    printf("1. add user\n"
           "2. find user\n"
           "3. delete user\n"
           "4. sort users by id\n"
           "5. sort users by name\n"
           "6. print users\n"
           "7. count users\n"
           "8. quit\n");

    command = get_int("Incorrect command", &ok);
    if (!ok) {
      continue;
    }

    if (command == 1) {
      printf("Enter id: ");
      id = get_int("Incorrect id", &ok);
      if (!ok) {
        continue;
      }

      printf("Enter name: ");
      get_string(name, sizeof(name));

      add_user(htable, id, name);

    } else if (command == 2) {
      printf("Enter id: ");
      id = get_int("Incorrect id", &ok);
      if (!ok) {
        continue;
      }

      struct user_info *user = find_user(htable, id);
      if (user) {
        print_user(user);
      } else {
        printf("User not found!\n");
      }

    } else if (command == 3) {
      printf("Enter id: ");
      id = get_int("Incorrect id", &ok);
      if (!ok) {
        continue;
      }

      delete_user(htable, id);
    } else if (command == 4) {
      htable_sort(htable, compare_by_id);
    } else if (command == 5) {
      htable_sort(htable, compare_by_name);
    } else if (command == 6) {
      print_users(htable);
    } else if (command == 7) {
      size_t count = htable_size(htable);
      printf("Count: %zu\n", count);
    } else if (command == 8) {
      should_run = false;
    }

    printf("\n");
  }

  return 0;
}
