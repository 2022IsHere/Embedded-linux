cmake_minimum_required(VERSION 3.16)
project(hello VERSION 0.1.0 LANGUAGES C CXX)

add_executable(lab1 hello.c)

include(GNUInstallDirs)
install(TARGETS lab1
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})