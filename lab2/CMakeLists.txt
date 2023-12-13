cmake_minimum_required(VERSION 3.16)
project(hello VERSION 0.1.0 LANGUAGES C CXX)

add_executable(lab2 hello.c)
install(TARGETS lab2
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
