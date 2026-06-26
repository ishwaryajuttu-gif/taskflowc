#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char title[100];
    char description[255];
    char due_date[20];
    int completed;
    struct Task* next;
} Task;

Task* head = NULL;

// Save all tasks to file
void save_tasks() {
    FILE* file = fopen("tasks.txt", "w");
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
void load_tasks() {
    FILE* file = fopen("tasks.txt", "r");
    if (file == NULL) return;
    char title[100], description[255], due_date[20];
    int completed;
    while (fscanf(file, "%99[^|]|%254[^|]|%19[^|]|%d\n",
        title, description, due_date, &completed) == 4) {
        Task* new_task = (Task*)malloc(sizeof(Task));
        strcpy(new_task->title, title);
        strcpy(new_task->description, description);
        strcpy(new_task->due_date, due_date);
        new_task->completed = completed;
        new_task->next = head;
        head = new_task;
    }
    fclose(file);
}

void add_task(char* title, char* description, char* due_date) {
    Task* new_task = (Task*)malloc(sizeof(Task));
    strcpy(new_task->title, title);
    strcpy(new_task->description, description);
    strcpy(new_task->due_date, due_date);
    new_task->completed = 0;
    new_task->next = head;
    head = new_task;
    printf("Task added: %s\n", title);
    save_tasks();
}

void display_tasks() {
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

void complete_task(char* title) {
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

void delete_task(char* title) {
    Task* current = head;
    Task* prev = NULL;
    while (current != NULL) {
        if (strcmp(current->title, title) == 0) {
            if (prev == NULL) head = current->next;
            else prev->next = current->next;
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