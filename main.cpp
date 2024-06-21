#include "Shell.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc > 1) {
        Shell shell(argv[1]);
        shell.run();
    } else {
        Shell shell;
        shell.run();
    }
    return 0;
}
