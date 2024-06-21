#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>

// Command 类用于处理单个命令的解析和执行
class Command {
public:
    Command(const std::string& commandLine, int in_fd = 0, int out_fd = 1);
    void execute();  
    bool is_empty() const;  

private:
    std::vector<std::string> args;  // 存储分解后的命令参数
    std::string input_file;  // 输入重定向文件
    std::string output_file;  // 输出重定向文件
    int input_fd;  // 输入文件描述符
    int output_fd;  // 输出文件描述符

    std::vector<char*> getCArgs() const;  // 获取用于 execvp 的参数数组
};

#endif
