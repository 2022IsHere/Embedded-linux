cmake_minimum_required(VERSION 3.16)
project(hello VERSION 0.1.0 LANGUAGES C CXX)

add_executable(lab4.1 main.c timers_init.c)
target_link_libraries(lab4.1 /var/lib/schroot/chroots/rpizero-bullseye-armhf/usr/lib/arm-linux-gnueabihf/librt.so)
target_link_libraries(lab4.1 /var/lib/schroot/chroots/rpizero-bullseye-armhf/usr/lib/arm-linux-gnueabihf/libgpiod.so)
include_directories(/var/lib/schroot/chroots/rpizero-bullseye-armhf/usr/local/include/) 
include_directories(/var/lib/schroot/chroots/rpizero-bullseye-armhf/usr/include/) 
include_directories(/var/lib/schroot/chroots/rpizero-bullseye-armhf/usr/include/arm-linux-gnueabihf/) 

include(GNUInstallDirs)
install(TARGETS lab4.1
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})