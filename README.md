Kaatrix: Implementation of a POSIX-Compliant Command-Line Interpreter
Author: Kaarthick Rajakumar
Date: December 2025

Subject: Systems Programming / Operating Systems

1. Abstract
This paper details the design and implementation of Kaatrix, a custom Unix-style shell developed in C++. The project explores the fundamental relationship between a user-space application and the OS kernel. By implementing core features such as process management, I/O redirection, and interactive terminal control, Kaatrix demonstrates the mechanics of command parsing and execution. A significant portion of the development focused on cross-platform compatibility, specifically adapting POSIX standards to a Windows-based environment via MSYS2.

2. Introduction
A shell serves as the primary interface for operating system interaction. While modern shells are highly complex, the core logic remains a loop: Read, Parse, Execute. Kaatrix was built to replicate this lifecycle while adding advanced user-experience extensions that define modern CLI environments.

3. Technical Implementation & Extensions
The development of Kaatrix was divided into four critical architectural milestones:

Process Navigation: Implementation of internal "built-ins" (cd, pwd) to manage the process state without spawning child processes.

Lexical Analysis & Quoting: Development of a robust parser capable of handling single (') and double (") quotes, as well as backslash (\) escaping to ensure literal character integrity.

I/O Stream Redirection: Utilizing system calls (dup2, open) to manipulate file descriptors, allowing the shell to redirect STDOUT and STDERR to external files.

Interactive Autocompletion: Implementation of a non-canonical terminal mode to intercept keystrokes. This allows for real-time PATH scanning and command suggestion via the <TAB> key.

4. Installation & Environment Setup
Kaatrix is designed for Unix-like environments but is fully functional on Windows through the MSYS2 translation layer.

Requirements:

Compiler: g++ (GCC)
Environment: Linux, macOS, or MSYS2 (MSYS Terminal recommended for sys/wait.h support).

Build Process:
Navigate to the source directory: cd path/to/Kaatrix/src
Compile the source: g++ Kaatrix.cpp -o Kaatrix.exe

Execute the shell: ./Kaatrix.exe

5. Challenges & Solutions
The primary technical challenge involved the implementation of <sys/wait.h> and <termios.h> on a Windows architecture. Windows does not natively support POSIX process management.
Solution: The project utilized the MSYS2 MSYS environment, which provides a DLL abstraction layer. This allows Unix system calls to be translated into Windows API calls at runtime, enabling the use of fork() and waitpid().

6. Future Implications
The current architecture of Kaatrix provides a foundation for several advanced systems-level features:

Piping: Implementing inter-process communication (IPC) via pipe() to chain commands.

Job Control: Adding background execution (&) and signal handling (SIGINT, SIGTSTP).

Scripting Support: Developing a control-flow logic for .ktx scripts (e.g., if/else loops).

7. Conclusion
Kaatrix successfully demonstrates that a functional, interactive shell can be built from fundamental C++ principles. The project highlights the importance of understanding low-level I/O, memory safety, and the nuances of cross-platform systems programming. Through this implementation, the developer has gained a profound understanding of how modern operating systems manage processes and user input.
