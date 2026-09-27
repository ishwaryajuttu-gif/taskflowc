# TaskFlow-C 🗂️

A command-line Task Manager application built in C, demonstrating core Data Structures and Algorithms (DSA) concepts including Linked Lists and File I/O.

## 📌 Features

- ✅ Add tasks with title, description, and due date
- 📋 List all tasks with status
- ✔️ Mark tasks as completed
- 🗑️ Delete tasks
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

### List all tasks
```bash
./taskflowc list
```

### Mark task as completed
```bash
./taskflowc complete "Task Title"
```

### Delete a task
```bash
./taskflowc delete "Task Title"
```

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

