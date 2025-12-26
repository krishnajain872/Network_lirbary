include(CheckIPOSupported)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
    STRINGS Debug Release RelWithDebInfo MinSizeRel ASan
)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Using ccache")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

add_library(network_options INTERFACE)

target_compile_features(network_options INTERFACE cxx_std_20)

target_compile_options(network_options INTERFACE
    $<$<CONFIG:Debug>:-O0 -g -Wall -Wextra -DDEBUG>
    $<$<CONFIG:Release>:-O3 -DNDEBUG -march=native>
    $<$<CONFIG:ASan>:-O1 -g -fsanitize=address -fno-omit-frame-pointer>
)

target_link_options(network_options INTERFACE
    $<$<CONFIG:ASan>:-fsanitize=address>
)

check_ipo_supported(RESULT ipo_supported)
if(ipo_supported AND CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
