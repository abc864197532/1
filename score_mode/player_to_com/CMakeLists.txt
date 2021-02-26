cmake_minimum_required(VERSION 3.17)
project(player_to_com)

set(CMAKE_CXX_STANDARD 14)
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/Users/smw93/Desktop/SFML-2.5.1/lib/cmake/SFML")

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)
add_executable(player_to_com main.cpp)
target_link_libraries(player_to_com sfml-graphics)