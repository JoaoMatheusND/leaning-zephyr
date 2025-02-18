cmake_minimum_required(VERSION 3.31)

set(BOARD sltb010a@2)

#set(DTC_OVERLAY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/boards/sltb010a.overlay)


find_package(Zephyr)
project(teste)

target_sources(app PRIVATE src/main.c)
