cmake_minimum_required(VERSION 3.24)
project(mho_launcher)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_GENERATOR "Visual Studio 16 2019")
#set(CMAKE_GENERATOR_PLATFORM Win32)

#set(CMAKE_C_FLAGS -m32)
#set(CMAKE_CXX_FLAGS -m32)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # Cannot be built for 64-bit (due to process .dll injection)
    message( FATAL_ERROR "This project cannot be built for 64-bit. Please target 32-bit." )
endif()

add_executable(mho_launcher_exe mho_launcher.cpp)
set_target_properties(mho_launcher_exe PROPERTIES OUTPUT_NAME "mho_launcher")

add_library(mho_launcher_dll SHARED mho_launcher_lib.cpp)
target_include_directories(mho_launcher_dll PRIVATE concurrentqueue/)
set_target_properties(mho_launcher_dll PROPERTIES OUTPUT_NAME "mho_launcher_lib")