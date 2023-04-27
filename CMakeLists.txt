cmake_minimum_required(VERSION 3.22.1)

project(analysers)

set(CMAKE_CXX_STANDARD 20)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

aux_source_directory(./src SRC_FILES)
include_directories(
    ${PROJECT_SOURCE_DIR}/include/
    /usr/include/mysql/
)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

add_executable(analyserslib main.cpp)
target_link_libraries(analyserslib
    ${PROJECT_NAME}
)