# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(oop_lab_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# ************ КЛЮЧОВА ЗМІНА: ПІДКЛЮЧЕННЯ GOOGLETEST ************
# Оскільки ми викликали add_subdirectory(tests) з кореня,
# ми повинні додати GoogleTest тут, якщо він не був доданий у корені.
# В основному файлі ми його прибрали, тому додаємо тут.

# Шлях до GoogleTest відносно кореня проєкту.
# NOTE: Це припускає, що "third_party/googletest" був завантажений у корені проєкту.
# Ми додаємо його тут, а не в основному CMakeLists.txt.

add_subdirectory(../third_party/googletest googletest-build)
# ***************************************************************

add_executable(tests_runner test_network.cpp)

# Додаємо директорії для заголовочних файлів (Graph.h, Network.h тощо)
target_include_directories(tests_runner PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Використовуємо modern targets для лінкування
target_link_libraries(tests_runner PRIVATE gtest gtest_main)
# Або, якщо ви використовуєте GoogleMock, використовуйте:
# target_link_libraries(tests_runner PRIVATE gmock gmock_main)
# Якщо вам потрібна повна підтримка GTest і GMock:
# target_link_libraries(tests_runner PRIVATE gmock_main)

include(GoogleTest)
gtest_discover_tests(tests_runner)