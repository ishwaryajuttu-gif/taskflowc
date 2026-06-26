In orderforthe application to run, you need to have GCC Compiler installed. Here's a simple guide:

### For Windows users:
1. Download and install **MinGW** from:
   `https://sourceforge.net/projects/mingw/`
2. During install, select **mingw32-gcc-g++**
3. Add to PATH: `C:\MinGW\bin`
4. Open **Git Bash** (not PowerShell)
5. Then run:
```bash
git clone https://github.com/ishwaryajuttu-gif/taskflowc.git
cd taskflowc
gcc src/task_manager.c src/main.c -o taskflowc
./taskflowc add "Buy milk" "grocery" "2026-06-30"
./taskflowc list
```

---

### For Mac users:
```bash
xcode-select --install
git clone https://github.com/ishwaryajuttu-gif/taskflowc.git
cd taskflowc
gcc src/task_manager.c src/main.c -o taskflowc
./taskflowc add "Buy milk" "grocery" "2026-06-30"
./taskflowc list
```

---

### For Linux users:
```bash
sudo apt install gcc
git clone https://github.com/ishwaryajuttu-gif/taskflowc.git
cd taskflowc
gcc src/task_manager.c src/main.c -o taskflowc
./taskflowc add "Buy milk" "grocery" "2026-06-30"
./taskflowc list
```

---


