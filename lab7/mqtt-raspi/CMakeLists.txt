cmake_minimum_required(VERSION 3.16)
project(mqtt VERSION 0.1.0 LANGUAGES C CXX)
set(SRC_FILES  
        mqtt-client.c
        gpio.c
) 
add_executable(mqtt ${SRC_FILES})

install(TARGETS mqtt
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

target_link_libraries(mqtt /var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/lib/arm-linux-gnueabihf/librt.so)
target_link_libraries(mqtt /var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/lib/arm-linux-gnueabihf/libgpiod.so)
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/local/include/) 
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/include/) 
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/include/arm-linux-gnueabihf/) 
include_directories(/var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/local/include/)
target_link_libraries(mqtt /var/lib/schroot/chroots/rpi3-bookworm-armhf/usr/local/lib/libpaho-mqtt3c.so )



