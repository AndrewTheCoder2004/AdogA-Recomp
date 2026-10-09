# Toolchain file for Windows Phone / Windows 10 Mobile (WinRT / UWP)
set(CMAKE_SYSTEM_NAME WindowsStore)
set(CMAKE_SYSTEM_VERSION 10.0)

# Target ARM for physical phones or x86 for emulators
if(NOT CMAKE_SYSTEM_PROCESSOR)
    set(CMAKE_SYSTEM_PROCESSOR arm)
endif()

# WinRT compilation definitions
add_compile_definitions(WINAPI_FAMILY=WINAPI_FAMILY_PHONE_APP)
add_compile_definitions(_WINPHONE=1)
add_compile_options(/ZW) # C++/CX syntax enabled for WinRT components
