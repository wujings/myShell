cmake_minimum_required(VERSION 3.5)

project(myShell LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(myShell main.cpp
Command.h Command.cpp
Shell.h Shell.cpp
)

include(GNUInstallDirs)
install(TARGETS myShell
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
