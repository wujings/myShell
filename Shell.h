#ifndef SHELL_H
#define SHELL_H

#include <string>

class Shell {
public:
    Shell();
    void run();
private:
    void executeLine(const std::string& line);
};

#endif
