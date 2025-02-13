cmake_minimum_required(VERSION 3.31)

set(BOARD sltb010a@2)

find_package(Zephyr)
project(teste)

target_sources(app PRIVATE src/main.c)