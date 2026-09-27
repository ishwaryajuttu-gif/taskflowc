#include <stdio.h>
#include <string.h>
#include "task_manager.h"

static void print_usage(void) {
    printf("Usage:\n");
    printf("  ./taskflowc add \"Title\" \"Description\" \"YYYY-MM-DD\"\n");
    printf("  ./taskflowc list\n");
    printf("  ./taskflowc complete \"Title\"\n");
    printf("  ./taskflowc delete \"Title\"\n");
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
    if (strcmp(argv[1], "add") == 0 && argc == 5) {
        add_task(argv[2], argv[3], argv[4]);
    } else if (strcmp(argv[1], "list") == 0 && argc == 2) {
        display_tasks();
    } else if (strcmp(argv[1], "complete") == 0 && argc == 3) {
        complete_task(argv[2]);
    } else if (strcmp(argv[1], "delete") == 0 && argc == 3) {
        delete_task(argv[2]);
    } else {
        print_usage();
        status = 1;
    }

    free_tasks();
    return status;
}
