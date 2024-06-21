#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>

class Command {
public:
    Command(const std::string& commandLine);
    void execute(int input_fd = 0, int output_fd = 1);
    bool is_empty() const;

private:
    std::vector<std::string> args;
    std::string input_file;
    std::string output_file;
    bool parse();
    std::vector<char*> getCArgs() const;
};

#endif
