#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>

// 分割字符串函数
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    return result;
}

// 执行单个命令
void executeCommand(const std::vector<std::string>& args, const std::string& infile, const std::string& outfile) {
    std::vector<char*> argv;
    for (const auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);

    pid_t pid = fork();
    if (pid == 0) {
        // 子进程，设置I/O重定向
        if (!infile.empty()) {
            int in_fd = open(infile.c_str(), O_RDONLY);
            if (in_fd == -1) {
                std::cerr << "Error opening input file." << std::endl;
                exit(EXIT_FAILURE);
            }
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        if (!outfile.empty()) {
            int out_fd = open(outfile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (out_fd == -1) {
                std::cerr << "Error opening output file." << std::endl;
                exit(EXIT_FAILURE);
            }
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }

        execvp(argv[0], argv.data());
        std::cerr << "Command execution failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // 父进程，等待子进程结束
        waitpid(pid, nullptr, 0);
    } else {
        std::cerr << "Failed to fork." << std::endl;
    }
}

// 解析命令行
void parseAndExecute(std::string commandLine) {
    std::istringstream iss(commandLine);
    std::string segment;
    std::vector<std::string> segments;
    while (std::getline(iss, segment, '|')) {
        segments.push_back(segment);
    }

    size_t num_commands = segments.size();
    int prev_fd[2] = {-1, -1};

    for (size_t i = 0; i < num_commands; ++i) {
        std::string command = segments[i];
        std::vector<std::string> parts = split(command, ' ');
        std::vector<std::string> args;
        std::string infile, outfile;

        for (size_t j = 0; j < parts.size(); ++j) {
            if (parts[j] == "<" && j + 1 < parts.size()) {
                infile = parts[++j];
            } else if (parts[j] == ">" && j + 1 < parts.size()) {
                outfile = parts[++j];
            } else {
                args.push_back(parts[j]);
            }
        }

        int fd[2];
        if (i < num_commands - 1) {  // Not the last command, create a pipe
            pipe(fd);
        }

        pid_t pid = fork();
        if (pid == 0) {
            if (!infile.empty()) {
                int in_fd = open(infile.c_str(), O_RDONLY);
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (i > 0) {  // Not the first command
                dup2(prev_fd[0], STDIN_FILENO);
                close(prev_fd[0]);
                close(prev_fd[1]);
            }
            if (i < num_commands - 1) {  // Not the last command
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }
            if (!outfile.empty()) {
                int out_fd = open(outfile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
                dup2(out_fd, STDOUT_FILENO);
                close(out_fd);
            }
            std::vector<char*> c_args;
            for (auto& arg : args) {
                c_args.push_back(&arg[0]);
            }
            c_args.push_back(nullptr);
            execvp(c_args[0], c_args.data());
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            if (i > 0) {
                close(prev_fd[0]);
                close(prev_fd[1]);
            }
            if (i < num_commands - 1) {
                prev_fd[0] = fd[0];
                prev_fd[1] = fd[1];
            }
            waitpid(pid, nullptr, 0);
        } else {
            std::cerr << "Failed to fork." << std::endl;
        }
    }
}

int main() {
    std::string line;
    while (true) {
        std::cout << "myshell> ";
        if (!std::getline(std::cin, line) || line == "exit") {
            break;
        }
        parseAndExecute(line);
    }
    return 0;
}
