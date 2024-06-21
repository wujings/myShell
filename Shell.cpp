#include "Shell.h"
#include "Command.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sstream> 
Shell::Shell(const std::string& scriptPath) : scriptPath(scriptPath) {}

void Shell::run() {
    std::istream* input = &std::cin;
    std::ifstream file;
    if (input == &std::cin) {
            std::cout << "myshell> ";
        }
    if (!scriptPath.empty()) {
        file.open(scriptPath);
        if (!file.is_open()) {
            std::cerr << "fail to open file: " << scriptPath << std::endl;
            return;
        }
        input = &file;
    }

    std::string line;
    while (std::getline(*input, line)) {
        if (line.empty() || line[0] == '#') continue; // 跳过空行和注释
        executeLine(line);
    }
}

void Shell::executeLine(const std::string& line) {
    std::vector<std::string> commands = split(line, '|');
    int num_commands = commands.size();
    int prev_fd = 0;

    for (int i = 0; i < num_commands; ++i) {
        int fd[2];
        pipe(fd);
        Command cmd(commands[i], prev_fd, (i < num_commands - 1) ? fd[1] : 1);
        cmd.execute();

        if (prev_fd != 0) close(prev_fd);
        close(fd[1]);
        prev_fd = fd[0];
    }
    if (prev_fd != 0) close(prev_fd);
}

std::vector<std::string> Shell::split(const std::string& str, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (getline(ss, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}
