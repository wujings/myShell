#include "Shell.h"
#include "Command.h"
#include <iostream>

Shell::Shell() {}

void Shell::run() {
    std::string line;
    while (true) {
        std::cout << "myshell> ";
        if (!std::getline(std::cin, line) || line == "exit") {
            break;
        }
        executeLine(line);
    }
}

void Shell::executeLine(const std::string& line) {
    Command cmd(line);
    cmd.execute();
}
