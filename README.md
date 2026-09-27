# TaskFlow-C 🗂️

[![Tests](https://github.com/ishwaryajuttu-gif/taskflowc/actions/workflows/tests.yml/badge.svg)](https://github.com/ishwaryajuttu-gif/taskflowc/actions/workflows/tests.yml)

A command-line Task Manager application built in C, demonstrating core Data Structures and Algorithms (DSA) concepts including Linked Lists and File I/O.

## 📌 Features

- ✅ Add tasks with title, description, and due date
- 🔢 Every task gets a numeric ID, so titles don't have to be unique
- 📋 List all tasks with their ID and status
- ✔️ Mark tasks as completed by ID
- 🗑️ Delete tasks by ID
- 💾 Persistent storage using file I/O (tasks.txt)

## 🛠️ Tech Stack

- Language: C
- Data Structure: Linked List
- Storage: File I/O (pipe-separated text file)
- Version Control: Git & GitHub

## 📂 Project Structure

```text
taskflowc/
├── src/
│   ├── task_manager.h   # Shared declarations and field size limits
│   ├── task_manager.c   # Core task logic & data structure
│   └── main.c           # CLI entry point
├── tests/
│   └── run_tests.sh     # Builds the program and runs all checks
├── tasks.txt            # Task storage, created at runtime (not committed)
├── .gitignore
├── LICENSE
└── README.md
```

## ⚙️ Installation & Setup

### Prerequisites
- GCC Compiler
  - Windows: Install [MinGW](https://sourceforge.net/projects/mingw/)
  - Mac: Run `xcode-select --install`
  - Linux: Run `sudo apt install gcc`
- Git

### Clone the Repository
```bash
git clone https://github.com/ishwaryajuttu-gif/taskflowc.git
cd taskflowc
```

### Compile
```bash
gcc src/task_manager.c src/main.c -o taskflowc
```

## 🚀 Usage

### Add a task
```bash
./taskflowc add "Task Title" "Description" "YYYY-MM-DD"
```

Titles can be up to 99 characters, descriptions up to 254 and due dates up to 19. The title can't be empty, and no field may contain `|` or a line break.

Each new task gets the next ID, one more than the highest ID in use, and `add` prints it.

### List all tasks
```bash
./taskflowc list
```

Example output:

```text
ID: 1 | Title: Buy milk | Due: 2026-06-30 | Done: No
ID: 2 | Title: Buy milk | Due: 2026-07-07 | Done: Yes
```

### Mark task as completed
```bash
./taskflowc complete 1
```

### Delete a task
```bash
./taskflowc delete 1
```

`complete` and `delete` take the task's ID from `list`, so two tasks with the same title can be told apart.

`tasks.txt` stores one task per line as `id|title|description|due_date|done`. Files saved by older versions, which have no ID field, still load: their tasks are numbered automatically, and the IDs are written the next time the file is saved.

## 🧪 Testing

Run the test script with bash (Git Bash on Windows):

```bash
bash tests/run_tests.sh
```

It builds the program into a temporary folder and checks every command, task IDs, input limits, older files without IDs, and handling of corrupt or Windows-style `tasks.txt` files. It prints `PASS` or `FAIL` for each check and exits with status 1 if anything fails.

GitHub Actions runs the same script on Linux and Windows for every pull request and every push to `main` (see `.github/workflows/tests.yml`).

## 💡 DSA Concepts Used

| Concept | Usage |
|---|---|
| Linked List | Dynamic task storage with insert/delete |
| File I/O | Persistent storage across sessions |
| Structs | Task data modelling |
| Pointers | Memory management |

## 📖 Git Workflow

- `main` — stable production branch
- `development` — active development branch
- Changes merged via Pull Requests

## 👩‍💻 Author

**Ishwarya**
- GitHub: [@ishwaryajuttu-gif](https://github.com/ishwaryajuttu-gif)

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

