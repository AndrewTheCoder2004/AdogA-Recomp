# Toolchain file for PlayStation Vita (VitaSDK)
if(NOT DEFINED ENV{VITASDK})
    message(FATAL_ERROR "VITASDK environment variable not set")
endif()

set(VITASDK "$ENV{VITASDK}")
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(VITA TRUE)

set(CMAKE_C_COMPILER "${VITASDK}/bin/arm-vita-eabi-gcc")
set(CMAKE_CXX_COMPILER "${VITASDK}/bin/arm-vita-eabi-g++")
set(CMAKE_AR "${VITASDK}/bin/arm-vita-eabi-ar" CACHE FILEPATH "Archiver")

set(VITA_FLAGS "-Wl,-q -march=armv7-a -mfpu=neon -mfloat-abi=hard -D__vita__")
set(CMAKE_C_FLAGS "${VITA_FLAGS} -O2 -Wall" CACHE STRING "C flags")
set(CMAKE_CXX_FLAGS "${VITA_FLAGS} -O2 -Wall" CACHE STRING "C++ flags")

include_directories(SYSTEM
    "${VITASDK}/arm-vita-eabi/include"
    "${VITASDK}/arm-vita-eabi/include/SDL2"
)
link_directories("${VITASDK}/arm-vita-eabi/lib")
