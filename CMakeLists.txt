cmake_minimum_required (VERSION 3.0.0)
project (inf_grafica LANGUAGES CXX)

#------------------------- FLAGS DE COMPILACIÓN --------------------------------

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++20 -save-temps -static-libstdc++")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Ofast -fno-exceptions -march=native")

#----------------------- ORGANIZACIÓN DE BINARIOS ------------------------------

set(FullOutputDir "${CMAKE_SOURCE_DIR}/bin/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${FullOutputDir}/static libs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${FullOutputDir}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FullOutputDir}")

#------------------- DECLARACIÓN DE OBJETOS COMPILABLES ------------------------

include_directories(include src test)

set(Executables
    path_tracer
    tone_mapper

    test/test_base_inverse_identity
    test/test_planetary_station
)

# Header Only
set(HeaderOnlyProjects
    queue
)

set(SubProjects
    
)

set(Libraries
    # ¡El orden importa!
    tone_mapping
    image
    color_spaces
    format/ppm
    format/bmp
    shapes
    ray_tracing
    path_tracing
    geometry
)

#------------------------ COMPILACIÓN DE BIBLIOTECAS ---------------------------

#Header only
foreach(Header IN LISTS HeaderOnlyProjects)
    include_directories("lib/${Header}/include")
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

#------------------------ COMPILACIÓN DE BIBLIOTECAS ---------------------------

foreach(Executable IN LISTS Executables)

    string(FIND ${Executable} "/" BarPos REVERSE)
    if (${BarPos} EQUAL -1)
        set(ExecutablePath "src/${Executable}.cpp")
        set(ExecutableName "${Executable}")
    else()
        MATH(EXPR BarPos "${BarPos} + 1")
        string(SUBSTRING ${Executable} ${BarPos} -1 ExecutableName)
        set(ExecutablePath "${Executable}.cpp")
    endif()

    # Ejecutable a generar
    add_executable(${ExecutableName} "${ExecutablePath}")

    foreach(Project IN LISTS SubProjects)
        # Vincular módulos eternos
        target_link_libraries(${ExecutableName} LINK_PUBLIC ${Project}) 
    endforeach()

    foreach(Lib IN LISTS LibNames)
        # Vincular módulos internos
        target_link_libraries(${ExecutableName} LINK_PUBLIC ${Lib}) 
    endforeach()

endforeach()