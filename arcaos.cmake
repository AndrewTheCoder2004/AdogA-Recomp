# Toolchain file for ArcaOS / OS/2 Warp
set(CMAKE_SYSTEM_NAME OS2)
set(ARCAOS TRUE)
set(OS2 TRUE)

# OS/2 GCC toolchain (Netlabs / ArcaOS EMX GCC)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# OS/2 specific linker flags and definitions
set(CMAKE_C_FLAGS "-Zomf -D__OS2__ -D__EMX__")
set(CMAKE_CXX_FLAGS "-Zomf -D__OS2__ -D__EMX__")

# SDL2 for OS/2 paths
if(DEFINED ENV{OS2DIR})
    set(CMAKE_FIND_ROOT_PATH "$ENV{OS2DIR}/usr")
endif()
