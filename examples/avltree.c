#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datastructs/avl_tree.h"

#include "helper.h"

struct user_info {
  int id;
  char name[31];

  struct avl_node ah;
};

void add_user(struct avl_root *root, int id, char *name) {
  struct avl_node **link = &root->avl_node;
  struct avl_node *parent = NULL;

  while (*link) {
    parent = *link;
    struct user_info *current = avl_entry(parent, struct user_info, ah);

    if (id < current->id) {
      link = &parent->left;
    } else if (id > current->id) {
      link = &parent->right;
    } else {
      /* User found */
      strcpy(current->name, name);
      return;
    }
  }

  /* User not found */
  struct user_info *user = malloc(sizeof(*user));
  user->id = id;
  strcpy(user->name, name);

  avl_link_node(&user->ah, parent, link);
  avl_restore_properties(parent, root);
}

struct user_info *find_user(struct avl_root *root, int id) {
  struct avl_node *node = root->avl_node;

  while (node) {
    struct user_info *current = avl_entry(node, struct user_info, ah);

    if (id < current->id) {
      node = node->left;
    } else if (id > current->id) {
      node = node->right;
    } else {
      return current;
    }
  }

  return NULL;
}

void delete_user(struct avl_root *root, int id) {
  struct user_info *user = find_user(root, id);

  if (user) {
    avl_erase(&user->ah, root);
    free(user);
  }
}

void print_user(struct user_info *user) {
  printf("{ id: %d, name: %s }\n", user->id, user->name);
}

void print_users(struct avl_root *root) {
  struct user_info *current;

  avl_for_each(root, current, ah) {
    print_user(current);
  }
}

int main(int argc, char **argv) {
  struct avl_root root = {NULL};

  bool should_run = true;
  bool ok;

  int command, id;
  char name[BUFSIZ];

  while (should_run) {
    printf("1. add user\n"
           "2. find user\n"
           "3. delete user\n"
           "4. print users\n"
           "5. quit\n");

    command = get_int("Incorrect command", &ok);
    if (!ok) {
      continue;
    }

    if (command == 1) {
      printf("Enter id: ");
      int id = get_int("Incorrect id", &ok);
      if (!ok) {
        continue;
      }

      printf("Enter name: ");
      get_string(name, sizeof(name));

      add_user(&root, id, name);

    } else if (command == 2) {
      printf("Enter id: ");
      int id = get_int("Incorrect id", &ok);
      if (!ok) {
        continue;
      }

      struct user_info *user = find_user(&root, id);
      if (user) {
        print_user(user);
      } else {
        printf("User not found!\n");
      }

    } else if (command == 3) {
      printf("Enter id: ");
      int id = get_int("Incorrect id", &ok);
      if (!ok) {
        continue;
      }

      delete_user(&root, id);
    } else if (command == 4) {
      print_users(&root);
    } else if (command == 5) {
      should_run = false;
    }

    printf("\n");
  }

  return 0;
}
