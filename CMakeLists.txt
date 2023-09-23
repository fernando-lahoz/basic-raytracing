cmake_minimum_required (VERSION 3.0.0)
project (inf_grafica LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++20 -save-temps")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

# Saber si estoy en un sistema de 32 o 64 bits
set(OSBitness 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OSBitness 64)
endif()

set(FullOutputDir "${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}${OSBitness}/${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${FullOutputDir}/static libs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${FullOutputDir}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FullOutputDir}")

include_directories(include src)

set(Executable main)

set(SubProjects
    #... NO DEBERÍA SER NECESARIO
)

set(Libraries
    # AÑADIR AQUÍ LAS BIBLIOTECAS
    geometry
    planetary_station
)

foreach(Project IN LISTS SubProjects)
    include_directories(${Project})
    if(NOT TARGET ${Project})
        add_subdirectory(${Project})
    endif()    
endforeach()

foreach(Lib IN LISTS Libraries)
    add_library(${Lib} "src/${Lib}.cpp")
endforeach()

# Ejecutable a generar
add_executable(${Executable} "src/${Executable}.cpp")

foreach(Project IN LISTS SubProjects)
    # Vincular módulos
    target_link_libraries(${Executable} ${Project}) 
endforeach()

foreach(Lib IN LISTS Libraries)
    # Vincular módulos
    target_link_libraries(${Executable} ${Lib}) 
endforeach()
