cmake_minimum_required(VERSION 3.16)
project(App VERSION 0.1.0 LANGUAGES C CXX)
set(SRC_FILES  
        main.c
        detect_servo.c
) 
add_executable(App ${SRC_FILES})

install(TARGETS App
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

target_link_libraries(App /var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/lib/arm-linux-gnueabihf/librt.so)
target_link_libraries(App /var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/lib/arm-linux-gnueabihf/libgpiod.so)
target_link_libraries(App pthread) 
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/local/include/) 
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/include/) 
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/include/arm-linux-gnueabihf/) 
