cmake_minimum_required(VERSION 3.22)
project(PerlinGDExtension LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Inclui o repositório oficial do Godot-CPP
add_subdirectory(godot-cpp)

# Define o nome da biblioteca
set(TARGET_NAME "perlin_noise_native")
add_library(${TARGET_NAME} SHARED
    perlin_wrapper.cpp
    register_types.cpp
)

# Conecta a biblioteca aos bindings do Godot
target_link_libraries(${TARGET_NAME} PRIVATE godot-cpp)

# 4. Otimizações de hardware
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${TARGET_NAME} PRIVATE -O3 -ffast-math)
    target_compile_definitions(${TARGET_NAME} PRIVATE NDEBUG)

    target_link_options(${TARGET_NAME} PRIVATE -static-libgcc -static-libstdc++ -static)
endif()

# Ativa o OpenMP para paralelismo na CPU
find_package(OpenMP REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE OpenMP::OpenMP_CXX)

# Configura a saída da biblioteca dinamicamente por Sistema Operacional
set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../godot_project/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../godot_project/bin"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".dll")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".so")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".dylib")
endif()
