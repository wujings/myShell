#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <vector>
// Shell 类管理命令行界面和命令执行
class Shell {
public:
    Shell(const std::string& scriptPath = "");
    void run();  // 运行 shell

private:
    std::string scriptPath;  // 脚本文件路径
    void executeLine(const std::string& line);  // 执行一行命令
    std::vector<std::string> split(const std::string& str, char delim);  // 分割字符串
};

#endif
