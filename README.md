# Kaatrix: A POSIX-Compliant Command-Line Interpreter

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/Language-C++-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows%20(MSYS2)-lightgrey.svg)]()

**Author:** Kaarthick Rajakumar  
**Date:** December 2025  
**Subject:** Systems Programming / Operating Systems  

---

## 1. Abstract
graph TD
    A[User Input] --> B{Is it Built-in?}
    B -- Yes --> C[Execute in Parent Process]
    B -- No --> D[fork child process]
    D --> E[execvp command]
    E --> F[waitpid in Parent]
    F --> A
**Kaatrix** is a custom Unix-style shell developed in C++ that explores the fundamental relationship between user-space applications and the OS kernel. By implementing core features such as process management, I/O redirection, and interactive terminal control, Kaatrix demonstrates the intricate mechanics of command parsing and execution. A primary focus of development was cross-platform compatibility, specifically adapting POSIX standards to Windows via the MSYS2 abstraction layer.

## 2. Architecture Overview
Kaatrix operates as a bridge between the user and the Kernel. When a command is entered, the shell forks a child process to maintain the stability of the parent shell.

1. **Read:** Captures raw input using a non-canonical terminal mode.
2. **Parse:** Lexical analysis to handle quotes and escapes.
3. **Execute:** Spawning child processes via `fork()` or executing internal built-ins.



---

## 3. Technical Features & Milestones

### 🚀 Process Navigation
Implementation of internal **built-ins** (e.g., `cd`, `pwd`) to manage the process state directly without spawning child processes, ensuring environment persistence.

### 🔍 Lexical Analysis & Quoting
A robust parser capable of maintaining literal character integrity by handling:
* Single and Double quotes.
* Backslash escaping for special characters.

### 📤 I/O Stream Redirection
Utilizes system calls (`dup2`, `open`) to manipulate file descriptors, allowing users to redirect `STDOUT` and `STDERR` to external files.



### ⌨️ Interactive Autocompletion
Implements a **non-canonical terminal mode** to intercept keystrokes in real-time. This enables:
* **PATH Scanning:** Searching directories for executable matches.
* **TAB Completion:** Real-time command suggestion.

---

## 4. Installation & Environment Setup

Kaatrix is optimized for Unix-like environments but is fully functional on Windows via **MSYS2**.

### Requirements
* **Compiler:** `g++` (GCC)
* **Environment:** Linux, macOS, or MSYS2 (Required for `sys/wait.h` support on Windows).

### Build Process
1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/YourUsername/Kaatrix.git](https://github.com/YourUsername/Kaatrix.git)
    cd Kaatrix/src
    ```
2.  **Compile the source:**
    ```bash
    g++ Kaatrix.cpp -o Kaatrix.exe
    ```
3.  **Execute the shell:**
    ```bash
    ./Kaatrix.exe
    ```

---

## 5. Challenges & Solutions: The Windows Barrier
The primary technical hurdle was implementing `<sys/wait.h>` and `<termios.h>` on a Windows architecture, which does not natively support POSIX process management.

* **The Solution:** The project utilizes the **MSYS2 MSYS environment**. This provides a DLL abstraction layer that translates Unix system calls into Windows API calls at runtime, enabling the use of `fork()` and `waitpid()`.

---

## 6. Roadmap & Future Implications
The modular architecture of Kaatrix allows for the future implementation of:
* **Piping:** Inter-process communication (IPC) via `pipe()` to chain commands.
* **Job Control:** Background execution (`&`) and signal handling (`SIGINT`, `SIGTSTP`).
* **Scripting Support:** Logic for `.ktx` scripts (e.g., if/else, loops).

## 7. Conclusion
Kaatrix demonstrates that a functional, interactive shell can be built from fundamental C++ principles. It highlights the importance of low-level I/O, memory safety, and cross-platform systems programming nuances.

---
**License**
Distributed under the MIT License. See `LICENSE` for more information.
