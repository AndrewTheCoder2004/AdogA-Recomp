# Toolchain file for Nintendo Wii (devkitPro devkitPPC)
if(NOT DEFINED ENV{DEVKITPRO})
    message(FATAL_ERROR "DEVKITPRO environment variable not set")
endif()

set(DEVKITPRO "$ENV{DEVKITPRO}")
set(DEVKITPPC "${DEVKITPRO}/devkitPPC")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR powerpc)
set(WII TRUE)

set(CMAKE_C_COMPILER "${DEVKITPPC}/bin/powerpc-eabi-gcc")
set(CMAKE_CXX_COMPILER "${DEVKITPPC}/bin/powerpc-eabi-g++")
set(CMAKE_AR "${DEVKITPPC}/bin/powerpc-eabi-ar" CACHE FILEPATH "Archiver")

set(WII_FLAGS "-mrvl -mcpu=750 -meabi -mhard-float -DGEKKO -DHW_RVL -D__wii__")
set(CMAKE_C_FLAGS "${WII_FLAGS} -O2 -Wall" CACHE STRING "C flags")
set(CMAKE_CXX_FLAGS "${WII_FLAGS} -O2 -Wall" CACHE STRING "C++ flags")

include_directories(SYSTEM
    "${DEVKITPRO}/libogc/include"
    "${DEVKITPRO}/portlibs/wii/include"
    "${DEVKITPRO}/portlibs/wii/include/SDL2"
)
link_directories(
    "${DEVKITPRO}/libogc/lib/wii"
    "${DEVKITPRO}/portlibs/wii/lib"
)
