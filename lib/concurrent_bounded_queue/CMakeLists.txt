cmake_minimum_required (VERSION 3.0.0)
project (concurrent_bounded_queue LANGUAGES CXX)

include_directories (include)

add_library (${PROJECT_NAME} INTERFACE)

target_include_directories (
    ${PROJECT_NAME}
    INTERFACE 
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_compile_features (
    ${PROJECT_NAME} 
    INTERFACE 
        cxx_std_11
)

target_link_libraries(${PROJECT_NAME} INTERFACE)
