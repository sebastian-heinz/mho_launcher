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
add_executable(ag_mho_exe ag_mho_exe/main.cpp)
target_include_directories(ag_mho_exe PRIVATE common/ ag_mho_exe/)
set_target_properties(ag_mho_exe PROPERTIES OUTPUT_NAME "ag_mho")

# --- Injected DLL ---
add_library(ag_mho_dll SHARED
    ag_mho_dll/dllmain.cpp
    ag_mho_dll/log.cpp
    ag_mho_dll/hooks_ods.cpp
    ag_mho_dll/backtrace_util.cpp
    ag_mho_dll/hooks_mhoclient.cpp
    ag_mho_dll/hooks_mho_handlers.cpp
    ag_mho_dll/hooks_crygame.cpp
    ag_mho_dll/hooks_protocalhandler.cpp
    ag_mho_dll/hooks_tenproxy.cpp
    ag_mho_dll/hooks_tersafe.cpp
)
target_include_directories(ag_mho_dll PRIVATE
    common/
    ag_mho_dll/
    ag_mho_exe/
    third_party/concurrentqueue/
)
set_target_properties(ag_mho_dll PROPERTIES OUTPUT_NAME "ag_mho" PREFIX "")

# --- TerSafe DLL ---
add_library(tersafe_dll SHARED
    tersafe_dll/tersafe.c
    tersafe_dll/tersafe.def
)
set_target_properties(tersafe_dll PROPERTIES OUTPUT_NAME "TerSafe" PREFIX "")
target_link_options(tersafe_dll PRIVATE -Wl,--kill-at)

# --- BugTrace DLL ---
add_library(bugtrace_dll SHARED
    bugtrace_dll/bugtrace.c
    bugtrace_dll/bugtrace.def
)
set_target_properties(bugtrace_dll PROPERTIES OUTPUT_NAME "BugTrace" PREFIX "")
target_link_libraries(bugtrace_dll PRIVATE dbghelp)
target_link_options(bugtrace_dll PRIVATE -Wl,--kill-at)
