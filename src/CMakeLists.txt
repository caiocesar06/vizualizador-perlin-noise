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
    src/perlin_wrapper.cpp
    src/ruido_base.cpp
    src/register_types.cpp
)

# Conecta a biblioteca aos bindings do Godot
target_link_libraries(${TARGET_NAME} PRIVATE godot-cpp)

# Otimizações de hardware
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${TARGET_NAME} PRIVATE -O3 -ffast-math)
    target_compile_definitions(${TARGET_NAME} PRIVATE NDEBUG)

    target_link_options(${TARGET_NAME} PRIVATE -static-libgcc -static-libstdc++)
endif()

# Ativa o OpenMP de forma segura
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_compile_options(${TARGET_NAME} PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_libraries(${TARGET_NAME} PRIVATE OpenMP::OpenMP_CXX)

    # AUTOMAÇÃO: Copiar libgomp-1.dll automaticamente no Windows
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libgomp-1.dll
                        OUTPUT_VARIABLE LIBGOMP_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(EXISTS ${LIBGOMP_PATH})
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${LIBGOMP_PATH}
                "${CMAKE_SOURCE_DIR}/../godot_project/bin/libgomp-1.dll"
                COMMENT "Copiando libgomp-1.dll automaticamente para a pasta do Godot..."
            )
        endif()
    endif()
else()
    message(WARNING "OpenMP nao encontrado. Compilando fallback em single-thread.")
endif()

# Configura a saída da biblioteca dinamicamente por SO
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
