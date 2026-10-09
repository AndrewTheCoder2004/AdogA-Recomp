# Toolchain file for Haiku OS
set(CMAKE_SYSTEM_NAME Haiku)
set(HAIKU TRUE)

# Default to GCC on Haiku
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# Haiku system paths
set(CMAKE_SYSTEM_PREFIX_PATH "/boot/system;/boot/home/config")
include_directories(SYSTEM "/boot/system/develop/headers" "/boot/system/develop/headers/SDL2")
link_directories("/boot/system/lib" "/boot/system/develop/lib")
