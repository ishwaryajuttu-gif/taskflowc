#include <stdio.h>
#include <string.h>
#include "task_manager.h"

static void print_usage(void) {
    printf("Usage:\n");
    printf("  ./taskflowc add \"Title\" \"Description\" \"YYYY-MM-DD\"\n");
    printf("  ./taskflowc list\n");
    printf("  ./taskflowc complete ID\n");
    printf("  ./taskflowc delete ID\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    // Refuse to run if tasks.txt is corrupt, so a later save can't overwrite it
    if (load_tasks() != 0) {
        printf("Fix or remove tasks.txt before continuing.\n");
        free_tasks();
        return 1;
    }

    int status = 0;
    int id;
    if (strcmp(argv[1], "add") == 0 && argc == 5) {
        if (add_task(argv[2], argv[3], argv[4]) != 0) status = 1;
    } else if (strcmp(argv[1], "list") == 0 && argc == 2) {
        display_tasks();
    } else if ((strcmp(argv[1], "complete") == 0 || strcmp(argv[1], "delete") == 0) && argc == 3) {
        if (parse_id(argv[2], &id) != 0) {
            printf("Error: ID must be a positive whole number (see 'list').\n");
            status = 1;
        } else if (strcmp(argv[1], "complete") == 0) {
            complete_task(id);
        } else {
            delete_task(id);
        }
    } else {
        print_usage();
        status = 1;
    }

    free_tasks();
    return status;
}
