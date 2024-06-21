#include "Command.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <string.h>
Command::Command(const std::string& commandLine, int in_fd, int out_fd) : input_fd(in_fd), output_fd(out_fd) {
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

void Command::execute() {
    if (is_empty()) return;

    pid_t pid = fork();
    if (pid == 0) {
        if (!input_file.empty()) {        // 处理输入重定向
            input_fd = open(input_file.c_str(), O_RDONLY);
            if (input_fd == -1) {
                std::cerr << "fail to open input file: " << input_file << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        dup2(input_fd, STDIN_FILENO);
        if (input_fd != 0) close(input_fd);

        if (!output_file.empty()) {        // 处理输出重定向
            output_fd = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (output_fd == -1) {
                std::cerr << "fail to open output file: " << output_file << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        dup2(output_fd, STDOUT_FILENO);
        if (output_fd != 1) close(output_fd);

        auto argv = getCArgs();        // 执行命令
        execvp(argv[0], argv.data());
        std::cerr << "fail to execute command: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        waitpid(pid, nullptr, 0);
    } else {
        std::cerr << "fail to create process" << std::endl;
    }
}

bool Command::is_empty() const {
    return args.empty();
}
