cmake_minimum_required(VERSION 3.16)
project(cron VERSION 0.1.0 LANGUAGES C CXX)
set(SRC_FILES  
        4.0/src/lab4-ledoff.c         
)
add_executable(lab4-ledoff ${SRC_FILES})
set(SRC_FILES  
        4.0/src/lab4-ledon.c         
) 
add_executable(lab4-ledon ${SRC_FILES})
install(TARGETS lab4-ledon lab4-ledoff
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/local/include/) 
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/include/) 
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/include/arm-linux-gnueabihf/) 
target_link_libraries(lab4-ledon /var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/lib/arm-linux-gnueabihf/libgpiod.so) 
target_link_libraries(lab4-ledoff /var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/lib/arm-linux-gnueabihf/libgpiod.so) 

