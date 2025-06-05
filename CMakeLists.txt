cmake_minimum_required(VERSION 3.10)
project(SAT)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Подключаем SFML
find_package(SFML REQUIRED COMPONENTS Graphics Window System)

# Создаем исполняемый файл
add_executable(sat main.cpp)

# Линкуем библиотеки SFML
target_link_libraries(sat PRIVATE sfml-graphics sfml-window sfml-system)