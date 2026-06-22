cmake_minimum_required(VERSION 3.13)
include(FetchContent)

if(CATNIP)
    catnip_package(play_event_exporter_delta DEFAULT release)
    catnip_add_preset(release TOOLSET 3ds BUILD_TYPE Release)
    return()
endif()

option(BUILD_HOST_TESTS "Build host-side unit tests" OFF)
if(BUILD_HOST_TESTS)
    project(play_event_exporter_host_tests LANGUAGES CXX)
    enable_testing()

    add_executable(test_smdh
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/host/test_smdh.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/smdh.cpp
    )
    target_include_directories(test_smdh PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)
    target_compile_features(test_smdh PRIVATE cxx_std_23)
    target_compile_options(test_smdh PRIVATE -Wall -Wextra -Werror)
    add_test(NAME smdh COMMAND test_smdh)

    add_executable(test_titlenames
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/host/test_titlenames.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/titlenames.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/smdh.cpp
    )
    target_include_directories(test_titlenames PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)
    target_compile_features(test_titlenames PRIVATE cxx_std_23)
    target_compile_options(test_titlenames PRIVATE -Wall -Wextra -Werror)
    add_test(NAME titlenames COMMAND test_titlenames)

    add_executable(test_outputlog
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/host/test_outputlog.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/outputlog.cpp
    )
    target_include_directories(test_outputlog PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)
    target_compile_features(test_outputlog PRIVATE cxx_std_23)
    target_compile_options(test_outputlog PRIVATE -Wall -Wextra -Werror)
    add_test(NAME outputlog COMMAND test_outputlog)

    return()
endif()

# Declare project and use gnu++23 (actually gnu++26) and gnu23
project(play_event_exporter_delta LANGUAGES C CXX ASM)
add_executable(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED True
    CXX_EXTENSIONS ON
    C_STANDARD 23
    C_STANDARD_REQUIRED True
    C_EXTENSIONS ON
)

# Generate symbol list and map information (useful for debugging)
dkp_target_generate_symbol_list(${PROJECT_NAME})

# Targets
ctr_generate_smdh(${PROJECT_NAME}.smdh
    NAME "PlayEvent Exporter Delta"
    DESCRIPTION "Exports PlayEvent history with title names"
    AUTHOR "(c) 2025 TuxSH - Aki Hakune"
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.png"
)

ctr_create_3dsx(${PROJECT_NAME}
    SMDH ${PROJECT_NAME}.smdh
)

# Dependencies (fmtlib)
FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        12.1.0
)
FetchContent_MakeAvailable(fmt)

target_link_libraries(${PROJECT_NAME} fmt::fmt)

# Options for code generation
target_compile_options(${PROJECT_NAME} PUBLIC
    # Common C/C++ options (__3DS__ is defined in platform file)
    # Some options already defined in CTR_ARCH_SETTINGS / __dkp_init_platform_settings

    -Wall
    -Wextra
    -Wno-missing-field-initializers
    #-Wno-unused-parameter

    -fdiagnostics-color=always # because of Ninja. Remove as needed
    -ftrivial-auto-var-init=pattern
    #-fshort-enums

    # C++ specific options
    # set(CMAKE_CXX_STANDARD 26) isn't working quite yet
    $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++26 -fno-exceptions -fno-rtti -fno-threadsafe-statics -fconcepts-diagnostics-depth=3>
)

# Source code files
# $ find source -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.s" \) | sed 's|^|${CMAKE_CURRENT_SOURCE_DIR}/|' | sort
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source/flockfile.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/outputlog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/ptmplays.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/smdh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/titlenames.cpp
)
