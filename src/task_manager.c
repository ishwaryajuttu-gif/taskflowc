#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task_manager.h"

#define TASKS_FILE "tasks.txt"
#define LINE_SIZE 512
#define MAX_FIELDS 5

typedef struct Task {
    int id;
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

// One more than the highest ID in use, or -1 if IDs have run out
static int next_id(void) {
    int max = 0;
    for (Task* current = head; current != NULL; current = current->next) {
        if (current->id > max) max = current->id;
    }
    return max == INT_MAX ? -1 : max + 1;
}

static Task* find_task(int id) {
    for (Task* current = head; current != NULL; current = current->next) {
        if (current->id == id) return current;
    }
    return NULL;
}

int parse_id(const char* text, int* id) {
    if (text[0] < '1' || text[0] > '9') return -1;
    int value = 0;
    for (const char* p = text; *p != '\0'; p++) {
        if (*p < '0' || *p > '9') return -1;
        int digit = *p - '0';
        if (value > (INT_MAX - digit) / 10) return -1;
        value = value * 10 + digit;
    }
    *id = value;
    return 0;
}

// Copy a string into a fixed-size buffer, failing if it doesn't fit
static int copy_field(char* dest, size_t dest_size, const char* src) {
    if (strlen(src) >= dest_size) return -1;
    strcpy(dest, src);
    return 0;
}

// Split line on '|' in place; returns the field count, or -1 if there are more than max
static int split_fields(char* line, char* fields[], int max) {
    int count = 0;
    fields[count++] = line;
    for (char* p = line; *p != '\0'; p++) {
        if (*p == '|') {
            if (count == max) return -1;
            *p = '\0';
            fields[count++] = p + 1;
        }
    }
    return count;
}

// Parse "id|title|description|due_date|completed" into task; empty text fields are allowed.
// Lines from older files have no id field; those tasks get id 0 and are numbered after loading.
static int parse_line(char* line, Task* task) {
    line[strcspn(line, "\r\n")] = '\0';

    char* fields[MAX_FIELDS];
    int count = split_fields(line, fields, MAX_FIELDS);
    char** field = fields;
    if (count == MAX_FIELDS) {
        if (parse_id(fields[0], &task->id) != 0) return -1;
        field++;
    } else if (count == MAX_FIELDS - 1) {
        task->id = 0;
    } else {
        return -1;
    }

    if (copy_field(task->title, sizeof(task->title), field[0]) != 0 ||
        copy_field(task->description, sizeof(task->description), field[1]) != 0 ||
        copy_field(task->due_date, sizeof(task->due_date), field[2]) != 0) {
        return -1;
    }

    if (strcmp(field[3], "0") == 0) task->completed = 0;
    else if (strcmp(field[3], "1") == 0) task->completed = 1;
    else return -1;

    return 0;
}

// Reject duplicate IDs, then number any tasks loaded without one
static int check_and_assign_ids(void) {
    for (Task* current = head; current != NULL; current = current->next) {
        if (current->id == 0) continue;
        for (Task* other = current->next; other != NULL; other = other->next) {
            if (other->id == current->id) {
                printf("Error: %s has more than one task with ID %d.\n", TASKS_FILE, current->id);
                return -1;
            }
        }
    }
    for (Task* current = head; current != NULL; current = current->next) {
        if (current->id != 0) continue;
        int id = next_id();
        if (id < 0) {
            printf("Error: no task IDs left.\n");
            return -1;
        }
        current->id = id;
    }
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
        fprintf(file, "%d|%s|%s|%s|%d\n",
            current->id,
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
    return check_and_assign_ids();
}

int add_task(const char* title, const char* description, const char* due_date) {
    if (title[0] == '\0') {
        printf("Error: title cannot be empty.\n");
        return -1;
    }
    if (validate_field("Title", title, TITLE_SIZE) != 0 ||
        validate_field("Description", description, DESCRIPTION_SIZE) != 0 ||
        validate_field("Due date", due_date, DUE_DATE_SIZE) != 0) {
        return -1;
    }

    int id = next_id();
    if (id < 0) {
        printf("Error: no task IDs left.\n");
        return -1;
    }
    Task* new_task = malloc(sizeof(Task));
    if (new_task == NULL) {
        printf("Error: out of memory.\n");
        return -1;
    }
    new_task->id = id;
    strcpy(new_task->title, title);
    strcpy(new_task->description, description);
    strcpy(new_task->due_date, due_date);
    new_task->completed = 0;
    append_task(new_task);
    printf("Task added: %s (ID %d)\n", title, id);
    save_tasks();
    return 0;
}

void display_tasks(void) {
    Task* current = head;
    if (current == NULL) {
        printf("No tasks found.\n");
        return;
    }
    while (current != NULL) {
        printf("ID: %d | Title: %s | Description: %s | Due: %s | Done: %s\n",
            current->id,
            current->title,
            current->description,
            current->due_date,
            current->completed ? "Yes" : "No");
        current = current->next;
    }
}

void complete_task(int id) {
    Task* task = find_task(id);
    if (task == NULL) {
        printf("Task not found.\n");
        return;
    }
    task->completed = 1;
    printf("Task marked complete: %s (ID %d)\n", task->title, id);
    save_tasks();
}

void delete_task(int id) {
    Task* current = head;
    Task* prev = NULL;
    while (current != NULL) {
        if (current->id == id) {
            if (prev == NULL) head = current->next;
            else prev->next = current->next;
            if (current == tail) tail = prev;
            printf("Task deleted: %s (ID %d)\n", current->title, id);
            free(current);
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
