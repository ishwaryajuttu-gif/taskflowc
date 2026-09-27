#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task_manager.h"

#define TASKS_FILE "tasks.txt"
#define LINE_SIZE 512

typedef struct Task {
    char title[TITLE_SIZE];
    char description[DESCRIPTION_SIZE];
    char due_date[DUE_DATE_SIZE];
    int completed;
    struct Task* next;
} Task;

Task* head = NULL;
Task* tail = NULL;

// Append a task to the end of the list so file order is preserved
static void append_task(Task* task) {
    task->next = NULL;
    if (tail == NULL) head = task;
    else tail->next = task;
    tail = task;
}

// Copy a field of known length into a fixed-size buffer
static int copy_field(char* dest, size_t dest_size, const char* src, size_t len) {
    if (len >= dest_size) return -1;
    memcpy(dest, src, len);
    dest[len] = '\0';
    return 0;
}

// Parse "title|description|due_date|completed" into task; empty fields are allowed
static int parse_line(char* line, Task* task) {
    line[strcspn(line, "\r\n")] = '\0';

    char* sep1 = strchr(line, '|');
    char* sep2 = sep1 ? strchr(sep1 + 1, '|') : NULL;
    char* sep3 = sep2 ? strchr(sep2 + 1, '|') : NULL;
    if (sep3 == NULL || strchr(sep3 + 1, '|') != NULL) return -1;

    if (copy_field(task->title, sizeof(task->title), line, sep1 - line) != 0 ||
        copy_field(task->description, sizeof(task->description), sep1 + 1, sep2 - sep1 - 1) != 0 ||
        copy_field(task->due_date, sizeof(task->due_date), sep2 + 1, sep3 - sep2 - 1) != 0) {
        return -1;
    }

    if (strcmp(sep3 + 1, "0") == 0) task->completed = 0;
    else if (strcmp(sep3 + 1, "1") == 0) task->completed = 1;
    else return -1;

    return 0;
}

// Reject input that would overflow a field or break the file format
static int validate_field(const char* name, const char* value, size_t size) {
    if (strlen(value) >= size) {
        printf("Error: %s is too long (max %d characters).\n", name, (int)(size - 1));
        return -1;
    }
    if (strpbrk(value, "|\r\n") != NULL) {
        printf("Error: %s cannot contain '|' or line breaks.\n", name);
        return -1;
    }
    return 0;
}

// Save all tasks to file
void save_tasks() {
    FILE* file = fopen(TASKS_FILE, "w");
    if (file == NULL) {
        printf("Error saving tasks.\n");
        return;
    }
    Task* current = head;
    while (current != NULL) {
        fprintf(file, "%s|%s|%s|%d\n",
            current->title,
            current->description,
            current->due_date,
            current->completed);
        current = current->next;
    }
    fclose(file);
}

// Load tasks from file
int load_tasks(void) {
    FILE* file = fopen(TASKS_FILE, "r");
    if (file == NULL) return 0;

    char line[LINE_SIZE];
    int line_number = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;
        if (strchr(line, '\n') == NULL && !feof(file)) {
            printf("Error: %s line %d is too long.\n", TASKS_FILE, line_number);
            fclose(file);
            return -1;
        }
        if (line[strspn(line, "\r\n")] == '\0') continue;  // skip blank lines

        Task* new_task = malloc(sizeof(Task));
        if (new_task == NULL) {
            printf("Error: out of memory.\n");
            fclose(file);
            return -1;
        }
        if (parse_line(line, new_task) != 0) {
            printf("Error: %s line %d is malformed.\n", TASKS_FILE, line_number);
            free(new_task);
            fclose(file);
            return -1;
        }
        append_task(new_task);
    }
    fclose(file);
    return 0;
}

void add_task(const char* title, const char* description, const char* due_date) {
    if (title[0] == '\0') {
        printf("Error: title cannot be empty.\n");
        return;
    }
    if (validate_field("Title", title, TITLE_SIZE) != 0 ||
        validate_field("Description", description, DESCRIPTION_SIZE) != 0 ||
        validate_field("Due date", due_date, DUE_DATE_SIZE) != 0) {
        return;
    }

    Task* new_task = malloc(sizeof(Task));
    if (new_task == NULL) {
        printf("Error: out of memory.\n");
        return;
    }
    strcpy(new_task->title, title);
    strcpy(new_task->description, description);
    strcpy(new_task->due_date, due_date);
    new_task->completed = 0;
    append_task(new_task);
    printf("Task added: %s\n", title);
    save_tasks();
}

void display_tasks(void) {
    Task* current = head;
    if (current == NULL) {
        printf("No tasks found.\n");
        return;
    }
    while (current != NULL) {
        printf("Title: %s | Due: %s | Done: %s\n",
            current->title,
            current->due_date,
            current->completed ? "Yes" : "No");
        current = current->next;
    }
}

void complete_task(const char* title) {
    Task* current = head;
    while (current != NULL) {
        if (strcmp(current->title, title) == 0) {
            current->completed = 1;
            printf("Task marked complete: %s\n", title);
            save_tasks();
            return;
        }
        current = current->next;
    }
    printf("Task not found.\n");
}

void delete_task(const char* title) {
    Task* current = head;
    Task* prev = NULL;
    while (current != NULL) {
        if (strcmp(current->title, title) == 0) {
            if (prev == NULL) head = current->next;
            else prev->next = current->next;
            if (current == tail) tail = prev;
            free(current);
            printf("Task deleted: %s\n", title);
            save_tasks();
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("Task not found.\n");
}

void free_tasks(void) {
    while (head != NULL) {
        Task* next = head->next;
        free(head);
        head = next;
    }
    tail = NULL;
}
