cmake_minimum_required(VERSION 3.2)
project(MarathonMatchScoring)

set(CMAKE_CXX_STANDARD 11)

set(Cling_DIR $ENV{CLING_DIR})
set(LLVM_INSTALL_PREFIX $ENV{LLVM_INSTALL_PREFIX})
set(NLOHMANN_JSON_INCLUDE_DIR  $ENV{NLOHMANN_JSON_INCLUDE_DIR})

add_compile_options(-D_GLIBCXX_USE_NANOSLEEP -DLLVMDIR="${LLVM_INSTALL_PREFIX}" -I${LLVM_INSTALL_PREFIX}/include)

add_executable(${PROJECT_NAME} src/main.cpp src/Runner.cpp src/Runner.h src/MemoryMonitor.cpp src/MemoryMonitor.h src/third-party/memory_usage.h)

find_package(Cling REQUIRED)
if (Cling_FOUND)
    message(STATUS ${Cling_INCLUDE_DIRS})
    message(STATUS ${Cling_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} clingInterpreter)
else()
    message(FATAL_ERROR "Cling not found!")
endif (Cling_FOUND)

include_directories(${NLOHMANN_JSON_INCLUDE_DIR})
