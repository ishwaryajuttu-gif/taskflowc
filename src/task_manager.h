#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#define TITLE_SIZE 100
#define DESCRIPTION_SIZE 255
#define DUE_DATE_SIZE 20

// Returns 0 on success, -1 if the task file is unreadable or corrupt
int load_tasks(void);
void add_task(const char* title, const char* description, const char* due_date);
void display_tasks(void);
void complete_task(const char* title);
void delete_task(const char* title);
void free_tasks(void);

#endif
