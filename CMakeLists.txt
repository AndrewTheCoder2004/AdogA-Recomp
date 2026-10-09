cmake_minimum_required(VERSION 3.16)
project(CadogAdventures CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/input.cpp
    src/audio.cpp
    src/game.cpp
)

set(HEADERS
    src/input.hpp
    src/audio.hpp
    src/formats.hpp
    src/game.hpp
)

# Optional SDL2_mixer for background music
find_package(SDL2_mixer CONFIG QUIET)
if(NOT SDL2_mixer_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2_MIXER QUIET SDL2_mixer)
    endif()
endif()

# Platform-specific configurations
if(ANDROID)
    # Android uses a shared library loaded by SDLActivity
    add_library(cadog SHARED ${SOURCES} ${HEADERS})
    find_library(ANDROID_LOG log)
    find_library(ANDROID_GLES GLESv2)
    target_link_libraries(cadog PRIVATE SDL2::SDL2 ${ANDROID_LOG} ${ANDROID_GLES})
elseif(IOS)
    add_executable(cadog MACOSX_BUNDLE ${SOURCES} ${HEADERS})
    set_target_properties(cadog PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.cadog.adventures"
        MACOSX_BUNDLE_BUNDLE_NAME "Cadog Adventures"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2" # iPhone & iPad
    )
    find_package(SDL2 REQUIRED)
    target_link_libraries(cadog PRIVATE SDL2::SDL2)
elseif(APPLE)
    # macOS Universal App Bundle
    add_executable(cadog MACOSX_BUNDLE ${SOURCES} ${HEADERS})
    set_target_properties(cadog PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.cadog.adventures"
        MACOSX_BUNDLE_BUNDLE_NAME "Cadog Adventures"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
    )
    find_package(SDL2 REQUIRED)
    target_link_libraries(cadog PRIVATE SDL2::SDL2)
elseif(WIN32 OR MINGW OR MSVC)
    add_executable(cadog WIN32 ${SOURCES} ${HEADERS})
    find_package(SDL2 CONFIG QUIET)
    if(NOT SDL2_FOUND)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(SDL2 REQUIRED sdl2)
        endif()
    endif()
    if(TARGET SDL2::SDL2)
        target_link_libraries(cadog PRIVATE SDL2::SDL2)
        if(TARGET SDL2::SDL2main)
            target_link_libraries(cadog PRIVATE SDL2::SDL2main)
        endif()
    else()
        target_include_directories(cadog PRIVATE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(cadog PRIVATE ${SDL2_LIBRARIES})
    endif()
elseif(VITA)
    # PlayStation Vita (VitaSDK)
    add_executable(cadog ${SOURCES} ${HEADERS})
    target_link_libraries(cadog PRIVATE SDL2 pthread SceGxm_stub SceCommonDialog_stub SceAudio_stub)
    if(COMMAND vita_create_self)
        vita_create_self(cadog.self cadog)
        vita_create_vpk(cadog.vpk "CADOG0001" cadog.self
            NAME "Cadog Adventures"
        )
    endif()
elseif(CTR OR N3DS)
    # Nintendo 3DS (devkitARM)
    add_executable(cadog ${SOURCES} ${HEADERS})
    target_link_libraries(cadog PRIVATE SDL2 ctru m)
    if(COMMAND ctr_create_3dsx)
        ctr_create_3dsx(cadog)
    endif()
elseif(WII)
    # Nintendo Wii (devkitPPC)
    add_executable(cadog.elf ${SOURCES} ${HEADERS})
    target_link_libraries(cadog.elf PRIVATE SDL2 ogc m)
    add_custom_target(cadog.dol ALL COMMAND elf2dol cadog.elf cadog.dol DEPENDS cadog.elf)
elseif(HAIKU)
    # Haiku OS
    add_executable(cadog ${SOURCES} ${HEADERS})
    find_package(SDL2 REQUIRED)
    target_link_libraries(cadog PRIVATE SDL2::SDL2 be)
elseif(ARCAOS OR OS2)
    # ArcaOS / OS/2
    add_executable(cadog ${SOURCES} ${HEADERS})
    find_package(SDL2 REQUIRED)
    target_link_libraries(cadog PRIVATE SDL2::SDL2)
elseif(XBOX360 OR LIBXENON)
    # Xbox 360 (libxenon)
    add_executable(cadog.elf ${SOURCES} ${HEADERS})
    target_link_libraries(cadog.elf PRIVATE SDL2 xenon m)
    add_custom_target(cadog.xex ALL COMMAND xenonxex -o cadog.xex cadog.elf DEPENDS cadog.elf)
elseif(PS3)
    # PlayStation 3 (PSL1GHT)
    add_executable(cadog.elf ${SOURCES} ${HEADERS})
    target_link_libraries(cadog.elf PRIVATE SDL2 psl1ght m)
    add_custom_target(EBOOT.BIN ALL COMMAND make_fself cadog.elf EBOOT.BIN DEPENDS cadog.elf)
else()
    # Linux, BSD, and standard POSIX targets
    add_executable(cadog ${SOURCES} ${HEADERS})
    find_package(SDL2 CONFIG QUIET)
    if(NOT SDL2_FOUND)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(SDL2 REQUIRED sdl2)
    endif()
    if(TARGET SDL2::SDL2)
        target_link_libraries(cadog PRIVATE SDL2::SDL2)
        if(TARGET SDL2::SDL2main)
            target_link_libraries(cadog PRIVATE SDL2::SDL2main)
        endif()
    else()
        target_include_directories(cadog PRIVATE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(cadog PRIVATE ${SDL2_LIBRARIES})
    endif()
endif()

# Link optional SDL2_mixer if detected
if(TARGET SDL2_mixer::SDL2_mixer)
    target_link_libraries(cadog PRIVATE SDL2_mixer::SDL2_mixer)
    target_compile_definitions(cadog PRIVATE HAVE_SDL_MIXER=1)
elseif(SDL2_MIXER_FOUND)
    target_include_directories(cadog PRIVATE ${SDL2_MIXER_INCLUDE_DIRS})
    target_link_libraries(cadog PRIVATE ${SDL2_MIXER_LIBRARIES})
    target_compile_definitions(cadog PRIVATE HAVE_SDL_MIXER=1)
endif()

target_include_directories(cadog PRIVATE src)
