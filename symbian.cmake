# Toolchain file for Symbian OS (S60 3rd/5th Edition, Symbian^3)
# Uses GCCE / CodeSourcery ARM EABI or SBSv2 with Open C/C++ P.I.P.S. POSIX runtime
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(SYMBIAN TRUE)

if(DEFINED ENV{EPOCROOT})
    set(EPOCROOT "$ENV{EPOCROOT}")
else()
    set(EPOCROOT "/Symbian/SDK")
endif()

if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER arm-none-symbianelf-gcc)
endif()
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER arm-none-symbianelf-g++)
endif()

set(SYMBIAN_FLAGS "-march=armv5t -mapcs -mthumb-interwork -D__SYMBIAN32__ -D__S60_50__ -D__EPOC32__")
set(CMAKE_C_FLAGS "${SYMBIAN_FLAGS} -O2" CACHE STRING "C flags")
set(CMAKE_CXX_FLAGS "${SYMBIAN_FLAGS} -O2" CACHE STRING "C++ flags")

include_directories(SYSTEM
    "${EPOCROOT}/epoc32/include"
    "${EPOCROOT}/epoc32/include/stdapis"
    "${EPOCROOT}/epoc32/include/stdapis/sys"
    "${EPOCROOT}/epoc32/include/SDL"
)
link_directories("${EPOCROOT}/epoc32/release/armv5/urel")
