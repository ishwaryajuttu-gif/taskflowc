#include <stdio.h>
#include <string.h>

void load_tasks();
void add_task(char* title, char* description, char* due_date);
void display_tasks();
void complete_task(char* title);
void delete_task(char* title);

int main(int argc, char* argv[]) {
    load_tasks();

    if (argc < 2) {
        printf("Usage: ./taskflowc <command>\n");
        printf("Commands: add, list, complete, delete\n");
        return 1;
    }

    if (strcmp(argv[1], "add") == 0) {
        add_task(argv[2], argv[3], argv[4]);
    } else if (strcmp(argv[1], "list") == 0) {
        display_tasks();
    } else if (strcmp(argv[1], "complete") == 0) {
        complete_task(argv[2]);
    } else if (strcmp(argv[1], "delete") == 0) {
        delete_task(argv[2]);
    } else {
        printf("Unknown command.\n");
    }

    return 0;
}