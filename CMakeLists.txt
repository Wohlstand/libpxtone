cmake_minimum_required (VERSION 3.2)
project(libpxtone VERSION 1.0.0 LANGUAGES C CXX)

include(GNUInstallDirs)
include(CheckCCompilerFlag)
include(TestBigEndian)

set(CMAKE_C_STANDARD 90)
set(CMAKE_CXX_STANDARD 98)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

#===========================================================================================

# Disable bogus MSVC warnings
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if(OPENBSD_LOCALBASE)
    message("OpenBSD Detected!")
    include_directories(${OPENBSD_LOCALBASE}/include)
endif()

if(NOT WIN32 AND CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    check_c_compiler_flag("-no-pie" HAS_NO_PIE)
endif()

function(set_nopie _target)
    if(NOT WIN32 AND HAS_NO_PIE AND CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        set_property(TARGET ${_target} APPEND_STRING PROPERTY LINK_FLAGS " -no-pie")
    endif()
endfunction()

function(set_legacy_standard destTarget)
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        # Turn on warnings and legacy C/C++ standards to support more compilers
        target_compile_options(${destTarget} PRIVATE
            $<$<COMPILE_LANGUAGE:C>:-Wall -pedantic -std=c90 -Wno-long-long>
            $<$<COMPILE_LANGUAGE:CXX>:-Wall -pedantic -std=gnu++98>
        )
    endif()
endfunction()

function(set_visibility_hidden destTarget)
    if(CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang)$" AND NOT DJGPP)
        if(CMAKE_VERSION VERSION_GREATER 3.2)
            target_compile_options(${destTarget} PRIVATE
              $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden> -fvisibility=hidden)
        else()
            target_compile_options(${destTarget} PRIVATE -fvisibility=hidden)
        endif()
        target_compile_definitions(${destTarget} PRIVATE LIBEDMIDI_VISIBILITY=)
    endif()
endfunction()

test_big_endian(THEXTECH_IS_BIG_ENDIAN)
if(THEXTECH_IS_BIG_ENDIAN)
    message("== Target processor endianess: BIG ENDIAN")
    set(IS_BIG_ENDIAN TRUE)
else()
    message("== Target processor endianess: LITTLE ENDIAN")
endif()
#===========================================================================================

option(WITH_PXPLAY          "Build also demo player (SDL2 is required)" OFF)

option(ENABLE_ADDRESS_SANITIZER "Enable the Address Sanitizer GCC feature" OFF)

if(NOT WIN32
   AND NOT VITA
   AND NOT NINTENDO_3DS
   AND NOT NINTENDO_WII
   AND NOT NINTENDO_WIIU
   AND NOT NINTENDO_SWITCH
   AND NOT EMSCRIPTEN
   AND NOT ADLMIDI_DOS)
    option(ENABLE_FPIC "Build static libraries as position independent code" ON)
else()
    set(ENABLE_FPIC OFF)
endif()

set(PXTONE_SRC
    src/pxtnData.cpp
    src/pxtnDelay.cpp
    src/pxtnError.cpp
    src/pxtnEvelist.cpp
    src/pxtnMaster.cpp
    src/pxtnMem.cpp
    src/pxtnOverDrive.cpp
    src/pxtnPulse_Frequency.cpp
    src/pxtnPulse_Noise.cpp
    src/pxtnPulse_NoiseBuilder.cpp
    src/pxtnPulse_Oggv.cpp
    src/pxtnPulse_Oscillator.cpp
    src/pxtnPulse_PCM.cpp
    src/pxtnService.cpp
    src/pxtnService_moo.cpp
    src/pxtnText.cpp
    src/pxtnUnit.cpp
    src/pxtnWoice.cpp
    src/pxtnWoice_io.cpp
    src/pxtnWoicePTV.cpp
    src/pxtoneNoise.cpp
)

# Make headers being properly visible at IDEs
file(GLOB PXTONE_HEADERS
    ${libpxtone_SOURCE_DIR}/src/*.h
)
list(APPEND PXTONE_SRC ${PXTONE_HEADERS})

#=================== STATIC LIBRARY ===================

add_library(libpxtone_static STATIC ${PXTONE_SRC})
if(WIN32 AND libEDMIDI_SHARED)
    set_target_properties(libpxtone_static PROPERTIES OUTPUT_NAME pxtone-static)
else()
    set_target_properties(libpxtone_static PROPERTIES OUTPUT_NAME pxtone)
endif()
target_include_directories(libpxtone_static PUBLIC ${libpxtone_SOURCE_DIR}/src)
set_legacy_standard(libpxtone_static)
set_visibility_hidden(libpxtone_static)

target_compile_definitions(libpxtone_static PRIVATE -DpxINCLUDE_OGGVORBIS -DpxINCLUDE_OGGVORBIS_STB)

if(IS_BIG_ENDIAN)
    target_compile_definitions(libpxtone_static PUBLIC -Dpx_BIG_ENDIAN)
endif()

if(ENABLE_ADDRESS_SANITIZER)
    target_compile_options(libpxtone_static PRIVATE -fsanitize=address)
endif()

# -fPIC thing
if(ENABLE_FPIC)
    set_target_properties(libpxtone_static PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
#=================== STATIC LIBRARY ===================


# === Unit tests ====
option(WITH_UNIT_TESTS "Enable unit testing" OFF)
if(WITH_UNIT_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()


if(WITH_PXPLAY)
    add_subdirectory(util/pxplay)
endif()
