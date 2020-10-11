cmake_minimum_required(VERSION 3.17)
project(big_tic_tac_toe)

set(CMAKE_CXX_STANDARD 14)
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/Users/smw93/Downloads/SFML-2.5.1/lib/cmake/SFML")

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)
add_executable(big_tic_tac_toe main.cpp)
target_link_libraries(big_tic_tac_toe sfml-graphics sfml-audio)