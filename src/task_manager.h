#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#define TITLE_SIZE 100
#define DESCRIPTION_SIZE 255
#define DUE_DATE_SIZE 20

// Returns 0 on success, -1 if the task file is unreadable or corrupt
int load_tasks(void);
// Returns 0 if the task was added, -1 if the input was rejected
int add_task(const char* title, const char* description, const char* due_date);
void display_tasks(void);
void complete_task(int id);
void delete_task(int id);
void free_tasks(void);

// Parse a positive task ID with no sign or leading zeros; returns 0 on success
int parse_id(const char* text, int* id);

#endif
