# Toolchain file for FreeBSD / OpenBSD / NetBSD
set(CMAKE_SYSTEM_NAME FreeBSD)

if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
endif()
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER clang++)
endif()

# BSD ports default include and library locations
list(APPEND CMAKE_PREFIX_PATH "/usr/local")
include_directories(SYSTEM "/usr/local/include" "/usr/local/include/SDL2")
link_directories("/usr/local/lib")
