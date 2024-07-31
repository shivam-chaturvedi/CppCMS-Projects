cmake_minimum_required(VERSION 3.22)
project(hello)

# Manually specify the paths to CppCMS includes and libraries
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

# Add the executable
add_executable(hello app.cpp)

# Link against the CppCMS and Booster libraries
target_link_libraries(hello cppcms booster)

