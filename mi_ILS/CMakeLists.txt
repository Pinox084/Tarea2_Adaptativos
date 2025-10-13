cmake_minimum_required(VERSION 3.15)
project(MI_ILS LANGUAGES CXX)

# --- Configuración general ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Ruta al proyecto del Local Search Solver ---
# Se asume que la carpeta 'localsearchsolver-master' está DENTRO de tu directorio de proyecto.
add_subdirectory(localsearchsolver-master ${CMAKE_BINARY_DIR}/local_build)

# --- Crear el ejecutable principal ---
# NO se deben añadir archivos .hpp aquí. Solo los .cpp que se compilan.
add_executable(mi_ILS main.cpp)

# --- Enlazar con la librería y especificar directorios de cabeceras (forma moderna) ---
target_link_libraries(mi_ILS
    PRIVATE
    LocalSearchSolver::localsearchsolver
)

target_include_directories(mi_ILS
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/localsearchsolver-master/include
)

# --- Mensaje informativo ---
message(STATUS "Proyecto MI_ILS configurado correctamente.")
message(STATUS "Desde la carpeta 'build', ejecuta: make && ./mi_ILS -i <archivo> -t <tiempo>")