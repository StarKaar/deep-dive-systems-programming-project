#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include <algorithm>

namespace fs = std::filesystem;
using namespace std;

string check_path(string command) {
    const char* path_env = std::getenv("PATH");
    if (!path_env) return "";
    string path_var = string(path_env);
    stringstream ss(path_var);
    string directory;
    while (getline(ss, directory, ':')) {
        fs::path full_path = fs::path(directory) / command;
        if (access(full_path.c_str(), X_OK) == 0) return full_path.string();
    }
    return "";
}

void set_raw_mode(struct termios& original) {
    struct termios raw = original;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

int main() {
    cout << "========================================" << endl;
    cout << "      Kaatrix v1.0              " << endl;
    cout << "      Extensions: Nav | Quote | Redir | Tab " << endl;
    cout << "========================================" << endl;
    cout << unitbuf;
    cerr << unitbuf;

    struct termios original_termios;
    tcgetattr(STDIN_FILENO, &original_termios);

    while (true) {
        cout << "$ " << flush;
        string cmd = "";
        char c;

        // 1. INPUT LOOP
        while (true) {
            set_raw_mode(original_termios);
            if (read(STDIN_FILENO, &c, 1) <= 0) break;
            tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);

            if (c == '\n') {
                cout << endl;
                break;
            }
            else if (c == '\t') {
                vector<string> builtins = { "echo", "exit", "type", "pwd", "cd" };
                vector<string> matches;

                // A. Check Builtins
                for (const string& b : builtins) {
                    if (b.size() >= cmd.size() && b.compare(0, cmd.size(), cmd) == 0) {
                        matches.push_back(b);
                    }
                }

                // B. Check PATH
                const char* path_env = getenv("PATH");
                if (path_env) {
                    string path_str = path_env; // FIX: Avoid Most Vexing Parse
                    stringstream ss(path_str);
                    string dir;
                    while (getline(ss, dir, ':')) {
                        if (dir.empty() || !fs::exists(dir)) continue;
                        try {
                            for (const auto& entry : fs::directory_iterator(dir)) {
                                string name = entry.path().filename().string();
                                if (name.size() >= cmd.size() && name.compare(0, cmd.size(), cmd) == 0) {
                                    if (access(entry.path().c_str(), X_OK) == 0) {
                                        matches.push_back(name);
                                    }
                                }
                            }
                        }
                        catch (...) {}
                    }
                }

                if (!matches.empty()) {
                    sort(matches.begin(), matches.end());
                    matches.erase(unique(matches.begin(), matches.end()), matches.end());

                    if (matches.size() == 1) {
                        string completion = matches[0].substr(cmd.size()) + " ";
                        cout << completion << flush;
                        cmd = matches[0] + " ";
                    }
                    else if (matches.size() > 1) {
                        // Optional: Could implement listing here, but for now just beep
                        cout << "\a" << flush;
                    }
                }
                else {
                    cout << "\a" << flush;
                }
            }
            else if (c == 127) {
                if (!cmd.empty()) { cmd.pop_back(); cout << "\b \b" << flush; }
            }
            else {
                cmd += c;
                cout << c << flush;
            }
        }
        if (cmd.empty()) continue;
        vector<string> args;
        string current_token;
        bool in_single_quotes = false, in_double_quotes = false;
        for (size_t i = 0; i < cmd.size(); ++i) {
            char ch = cmd[i];
            if (ch == '\\' && !in_single_quotes) {
                if (in_double_quotes) {
                    if (i + 1 < cmd.size() && (cmd[i + 1] == '"' || cmd[i + 1] == '\\' || cmd[i + 1] == '$')) current_token += cmd[++i];
                    else current_token += ch;
                }
                else if (i + 1 < cmd.size()) current_token += cmd[++i];
            }
            else if (ch == '\'' && !in_double_quotes) in_single_quotes = !in_single_quotes;
            else if (ch == '"' && !in_single_quotes) in_double_quotes = !in_double_quotes;
            else if (isspace(ch) && !in_single_quotes && !in_double_quotes) {
                if (!current_token.empty()) { args.push_back(current_token); current_token.clear(); }
            }
            else current_token += ch;
        }
        if (!current_token.empty()) args.push_back(current_token);
        if (args.empty()) continue;
        string out_file = "", err_file = "";
        bool out_append = false, err_append = false;
        int original_stdout = -1, original_stderr = -1;

        for (size_t i = 0; i < args.size(); ++i) {
            string op = args[i];
            bool is_out = (op == ">" || op == "1>" || op == ">>" || op == "1>>");
            bool is_err = (op == "2>" || op == "2>>");
            if ((is_out || is_err) && i + 1 < args.size()) {
                if (is_out) { out_file = args[i + 1]; out_append = (op.find(">>") != string::npos); }
                else { err_file = args[i + 1]; err_append = (op.find(">>") != string::npos); }
                args.erase(args.begin() + i, args.begin() + i + 2);
                i--;
            }
        }
        if (!out_file.empty()) {
            original_stdout = dup(STDOUT_FILENO);
            int flags = O_WRONLY | O_CREAT | (out_append ? O_APPEND : O_TRUNC);
            int fd = open(out_file.c_str(), flags, 0644);
            if (fd != -1) { dup2(fd, STDOUT_FILENO); close(fd); }
        }
        if (!err_file.empty()) {
            original_stderr = dup(STDERR_FILENO);
            int flags = O_WRONLY | O_CREAT | (err_append ? O_APPEND : O_TRUNC);
            int fd = open(err_file.c_str(), flags, 0644);
            if (fd != -1) { dup2(fd, STDERR_FILENO); close(fd); }
        }
        if (args[0] == "exit") {
            if (original_stdout != -1) { dup2(original_stdout, STDOUT_FILENO); close(original_stdout); }
            if (original_stderr != -1) { dup2(original_stderr, STDERR_FILENO); close(original_stderr); }
            break;
        }
        else if (args[0] == "echo") {
            for (size_t i = 1; i < args.size(); ++i) cout << args[i] << (i == args.size() - 1 ? "" : " ");
            cout << endl;
        }
        else if (args[0] == "pwd") { cout << fs::current_path().string() << endl; }
        else if (args[0] == "cd") {
            string target = (args.size() > 1) ? args[1] : "~";
            if (target == "~") { const char* home = getenv("HOME"); if (home) fs::current_path(home); }
            else { try { fs::current_path(target); } catch (...) { cout << "cd: " << target << ": No such file or directory" << endl; } }
        }
        else if (args[0] == "type") {
            if (args.size() > 1) {
                string t = args[1];
                if (t == "echo" || t == "exit" || t == "type" || t == "pwd" || t == "cd") cout << t << " is a shell builtin" << endl;
                else { string p = check_path(t); if (!p.empty()) cout << t << " is " << p << endl; else cout << t << ": not found" << endl; }
            }
        }
        else {
            string path = check_path(args[0]);
            if (!path.empty()) {
                pid_t pid = fork();
                if (pid == 0) {
                    vector<char*> c_args;
                    for (const string& s : args) c_args.push_back(const_cast<char*>(s.c_str()));
                    c_args.push_back(nullptr);
                    execv(path.c_str(), c_args.data());
                    exit(1);
                }
                else waitpid(pid, nullptr, 0);
            }
            else cout << args[0] << ": command not found" << endl;
        }

        if (original_stdout != -1) { dup2(original_stdout, STDOUT_FILENO); close(original_stdout); }
        if (original_stderr != -1) { dup2(original_stderr, STDERR_FILENO); close(original_stderr); }
    }
    return 0;
}