cmake_minimum_required(VERSION 3.12)   

project(threadPool)               

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")        

include_directories(include)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

# 添加源文件
set(LIB_SRC
    src/thread.cc
)

add_library(threadPool ${LIB_SRC})

set(LIB_LIB
    threadPool
    pthread    
)

# 生成可执行文件
add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread threadPool)
target_link_libraries(test_thread ${LIB_LIB})

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)