# Toolchain file for Nintendo 3DS (devkitPro devkitARM + libctru)
if(NOT DEFINED ENV{DEVKITPRO})
    message(FATAL_ERROR "DEVKITPRO environment variable not set")
endif()

set(DEVKITPRO "$ENV{DEVKITPRO}")
set(DEVKITARM "${DEVKITPRO}/devkitARM")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR armv6k)
set(CTR TRUE)
set(N3DS TRUE)

set(CMAKE_C_COMPILER "${DEVKITARM}/bin/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${DEVKITARM}/bin/arm-none-eabi-g++")
set(CMAKE_AR "${DEVKITARM}/bin/arm-none-eabi-ar" CACHE FILEPATH "Archiver")

set(CTR_FLAGS "-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft -DARM11 -D_3DS -D__3DS__")
set(CMAKE_C_FLAGS "${CTR_FLAGS} -O2 -Wall" CACHE STRING "C flags")
set(CMAKE_CXX_FLAGS "${CTR_FLAGS} -O2 -Wall" CACHE STRING "C++ flags")

include_directories(SYSTEM
    "${DEVKITPRO}/libctru/include"
    "${DEVKITPRO}/portlibs/3ds/include"
    "${DEVKITPRO}/portlibs/3ds/include/SDL2"
)
link_directories(
    "${DEVKITPRO}/libctru/lib"
    "${DEVKITPRO}/portlibs/3ds/lib"
)
