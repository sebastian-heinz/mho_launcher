cmake_minimum_required(VERSION 3.24)
project(mho_launcher)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "This project cannot be built for 64-bit. Please target 32-bit.")
endif()

if(MINGW)
    add_compile_options(-O0)
    add_link_options(-static -static-libgcc -static-libstdc++)
endif()

# --- Launcher executable ---
add_executable(mho_launcher_exe launcher/main.cpp)
target_include_directories(mho_launcher_exe PRIVATE common/ launcher/)
set_target_properties(mho_launcher_exe PROPERTIES OUTPUT_NAME "mho_launcher")

# --- Injected DLL ---
add_library(mho_launcher_dll SHARED
    dll/dllmain.cpp
    dll/log.cpp
    dll/hooks_mhoclient.cpp
    dll/hooks_crygame.cpp
    dll/hooks_protocalhandler.cpp
    dll/hooks_tenproxy.cpp
)
target_include_directories(mho_launcher_dll PRIVATE
    common/
    data/
    dll/
    launcher/
    third_party/concurrentqueue/
)
set_target_properties(mho_launcher_dll PROPERTIES OUTPUT_NAME "mho_launcher_lib" PREFIX "")
