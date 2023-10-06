cmake_minimum_required (VERSION 3.0.0)
project (inf_grafica LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++20 -save-temps -static-libstdc++")
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

include_directories(include src test)

set(Executable tone_mapper)

set(SubProjects
    #... NO DEBERÍA SER NECESARIO
)

set(Libraries
    # AÑADIR AQUÍ LAS BIBLIOTECAS
    color_spaces
    geometry
    tone_mapping
    image
    format/ppm
)

foreach(Project IN LISTS SubProjects)
    include_directories(${Project})
    if(NOT TARGET ${Project})
        add_subdirectory(${Project})
    endif()    
endforeach()

foreach(Lib IN LISTS Libraries)
    string(FIND ${Lib} "/" BarPos REVERSE)
    if (${BarPos} EQUAL -1)
        set(LibName ${Lib})
        set(LibNames "${LibNames};${LibName}")
    else()
        MATH(EXPR BarPos "${BarPos} + 1")
        string(SUBSTRING ${Lib} ${BarPos} -1 LibName)
        set(LibNames "${LibNames};${LibName}")
    endif()
    add_library(${LibName} STATIC "src/${Lib}.cpp")
endforeach()

find_file (ExecutablePath "${Executable}.cpp" PATHS src test)

# Ejecutable a generar
add_executable(${Executable} "${ExecutablePath}")

foreach(Project IN LISTS SubProjects)
    # Vincular módulos
    target_link_libraries(${Executable} ${Project}) 
endforeach()

foreach(Lib IN LISTS LibNames)
    # Vincular módulos
    target_link_libraries(${Executable} ${Lib}) 
endforeach()

#Inluir path distinto de libstdc++
#set(LibStdPath libstd_path.txt)
#if(EXISTS "${CMAKE_SOURCE_DIR}/${LibStdPath}")
#    file(READ "${CMAKE_SOURCE_DIR}/${LibStdPath}" LIBSTD)
#    message(STATUS "Using static lib: ${LIBSTD}")
#    target_link_libraries(${Executable} "${LIBSTD}") 
#endif()
