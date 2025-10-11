cmake_minimum_required(VERSION 3.15)
project(MI_ILS LANGUAGES CXX)

# Estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Incluir el proyecto de Local Search Solver (ya incluye optimizationtools)
add_subdirectory(${CMAKE_SOURCE_DIR}/../localsearchsolver-master ${CMAKE_BINARY_DIR}/local_build)

# Crear tu ejecutable
add_executable(mi_ILS main.cpp)

# Enlazar solo con el target exportado
target_link_libraries(mi_ILS
    PRIVATE
    LocalSearchSolver::localsearchsolver
)