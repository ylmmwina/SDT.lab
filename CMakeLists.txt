cmake_minimum_required(VERSION 3.29)
project(lab1_sem1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Додаємо всі файли, які є частиною основної бібліотеки/додатку
add_executable(lab1_sem1
        main.cpp
        Graph.h
        GraphAlgorithms.h
        Network.h
        NetworkSimulator.h
)

# 2. Додаємо піддиректорію тестів, яка створить окремий виконуваний файл tests_runner
add_subdirectory(tests)