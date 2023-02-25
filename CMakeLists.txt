cmake_minimum_required(VERSION 3.24)
set(CMAKE_GENERATOR_PLATFORM x64)
include(FetchContent)

project(freesight CXX)
include(cmake/StandardProjectSettings.cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_23)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

set(CMAKE_CXX_STANDARD 23)
option(CURL_USE_SCHANNEL "Use windows default SSL libraries" ON)
option(CMAKE_USE_WINSSL "Set to ON to use WINSSL for windows." ON)

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

add_executable(freesight main.cpp targetver.h stdafx.cpp stdafx.h firewall/windows_firewall.cpp firewall/windows_firewall.h logger/logger.h logger/color.h logger/banner.h config/config.h lib/split.h lib/elevate.h res/metadata.rc vpn/installer.h http/manager.cpp http/manager.h regedit/regedit.h regedit/regedit.cpp)
target_precompile_headers(freesight PUBLIC stdafx.h)

FetchContent_Declare(yaml GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
        GIT_TAG 0579ae3d976091d7d664aa9d2527e0d0cff25763) # https://github.com/jbeder/yaml-cpp/releases
FetchContent_MakeAvailable(yaml)

FetchContent_Declare(zlib GIT_REPOSITORY https://github.com/intel/zlib.git
        GIT_TAG 6160a8f20c3626aec2f8b0fda5bf2e65bfe31781) # https://github.com/curl/curl/releases
FetchContent_MakeAvailable(zlib)

FetchContent_Declare(curl GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG 046209e561b7e9b5aab1aef7daebf29ee6e6e8c7) # https://github.com/curl/curl/releases
FetchContent_MakeAvailable(curl)


target_link_libraries(freesight PRIVATE yaml-cpp zlibstatic libcurl project_options project_warnings)

if (WIN32)
    add_custom_command(
            TARGET freesight POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:freesight> $<TARGET_FILE_DIR:freesight>
            COMMAND_EXPAND_LISTS
    )
endif ()

include_directories( ${YAML_CPP_INCLUDE_DIR} )
include_directories( ${CURL_INCLUDE_DIR} )
include_directories( ${ZLIB_INCLUDE_DIRS} )

