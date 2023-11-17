cmake_minimum_required (VERSION 3.0.0)
project (progress_bar LANGUAGES CXX)

include_directories (include)

add_library (${PROJECT_NAME} src/text_progress_bar.cpp)
