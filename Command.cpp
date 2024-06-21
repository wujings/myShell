#include "Command.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <string.h>
Command::Command(const std::string& commandLine) {
    std::istringstream stream(commandLine);
    std::string token;
    while (stream >> token) {
        if (token == "<") {
            if (stream >> token) input_file = token;
        } else if (token == ">") {
            if (stream >> token) output_file = token;
        } else {
            args.push_back(token);
        }
    }
}

std::vector<char*> Command::getCArgs() const {
    std::vector<char*> cargs;
    for (const auto& arg : args) {
        cargs.push_back(const_cast<char*>(arg.c_str()));
    }
    cargs.push_back(nullptr);
    return cargs;
}

void Command::execute(int input_fd, int output_fd) {
    if (is_empty()) return;

    pid_t pid = fork();
    if (pid == 0) {
        if (!input_file.empty()) {
            int fd = open(input_file.c_str(), O_RDONLY);
            if (fd == -1) {
                std::cerr << "Failed to open input file: " << input_file << std::endl;
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        if (!output_file.empty()) {
            int fd = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd == -1) {
                std::cerr << "Failed to open output file: " << output_file << std::endl;
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        auto argv = getCArgs();
        execvp(argv[0], argv.data());
        std::cerr << "Command execution failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        waitpid(pid, nullptr, 0);
    } else {
        std::cerr << "Failed to fork." << std::endl;
    }
}

bool Command::is_empty() const {
    return args.empty();
}
