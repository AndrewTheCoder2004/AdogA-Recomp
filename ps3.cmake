# Toolchain file for PlayStation 3 (PSL1GHT open toolchain)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR powerpc64)
set(PS3 TRUE)

if(DEFINED ENV{PS3DEV})
    set(PS3DEV "$ENV{PS3DEV}")
else()
    set(PS3DEV "/usr/local/ps3dev")
endif()

set(CMAKE_C_COMPILER "${PS3DEV}/ppu/bin/ppu-gcc")
set(CMAKE_CXX_COMPILER "${PS3DEV}/ppu/bin/ppu-g++")
set(CMAKE_AR "${PS3DEV}/ppu/bin/ppu-ar" CACHE FILEPATH "Archiver")

set(PS3_FLAGS "-mcpu=cell -mhard-float -D__PS3__ -D__PSL1GHT__")
set(CMAKE_C_FLAGS "${PS3_FLAGS} -O2 -Wall" CACHE STRING "C flags")
set(CMAKE_CXX_FLAGS "${PS3_FLAGS} -O2 -Wall" CACHE STRING "C++ flags")

include_directories(SYSTEM
    "${PS3DEV}/portlibs/ppu/include"
    "${PS3DEV}/portlibs/ppu/include/SDL2"
    "${PS3DEV}/ppu/include"
)
link_directories(
    "${PS3DEV}/portlibs/ppu/lib"
    "${PS3DEV}/ppu/lib"
)
