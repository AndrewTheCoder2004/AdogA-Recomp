# Toolchain file for Microsoft Xbox 360 (libxenon toolchain)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR powerpc)
set(XBOX360 TRUE)
set(LIBXENON TRUE)

if(DEFINED ENV{DEVKITXENON})
    set(XENON_PREFIX "$ENV{DEVKITXENON}/bin/xenon-")
else()
    set(XENON_PREFIX "xenon-")
endif()

set(CMAKE_C_COMPILER "${XENON_PREFIX}gcc")
set(CMAKE_CXX_COMPILER "${XENON_PREFIX}g++")
set(CMAKE_AR "${XENON_PREFIX}ar" CACHE FILEPATH "Archiver")

set(XENON_FLAGS "-mcpu=cell -mtune=cell -m32 -fno-pic -mpowerpc64 -DXBOX360 -D__LIBXENON__")
set(CMAKE_C_FLAGS "${XENON_FLAGS} -O2 -Wall" CACHE STRING "C flags")
set(CMAKE_CXX_FLAGS "${XENON_FLAGS} -O2 -Wall" CACHE STRING "C++ flags")

if(DEFINED ENV{DEVKITXENON})
    include_directories(SYSTEM
        "$ENV{DEVKITXENON}/usr/include"
        "$ENV{DEVKITXENON}/usr/include/SDL2"
    )
    link_directories("$ENV{DEVKITXENON}/usr/lib")
endif()
