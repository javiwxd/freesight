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

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

add_executable(freesight main.cpp
        res/metadata.rc res/icon.rc
        targetver.h stdafx.c stdafx.h firewall/windows_firewall.cpp firewall/windows_firewall.h logger/logger.h logger/color.h logger/banner.h config/config.h lib/split.h lib/elevate.h vpn/installer.h http/manager.cpp http/manager.h regedit/regedit.h regedit/regedit.cpp updater/get_version.h updater/updater.h http/http_get.h lib/wzip.h lib/zip/libzippp.cpp lib/zip/libzippp.h)
target_precompile_headers(freesight PUBLIC stdafx.h)

set(CURL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/curl)
set(YAML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/yaml-cpp)
#set(CPR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpr)
set(JSON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/jsoncpp)
set(LIBZIP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/libzip)

#target_include_directories(freesight PRIVATE ${CURL_DIR}/include ${YAML_DIR}/include ${CPR_DIR}/include ${JSON_DIR}/include ${LIBZIP_DIR}/lib)
#target_link_directories(freesight PRIVATE ${CURL_DIR}/lib ${YAML_DIR}/lib ${CPR_DIR} ${JSON_DIR}/lib ${LIBZIP_DIR}/lib)

target_include_directories(freesight PRIVATE ${CURL_DIR}/include ${YAML_DIR}/include ${JSON_DIR}/include ${LIBZIP_DIR}/lib)
target_link_directories(freesight PRIVATE ${CURL_DIR}/lib ${YAML_DIR}/lib ${JSON_DIR}/lib ${LIBZIP_DIR}/lib)
###
#FetchContent_Declare(yaml GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
#        GIT_TAG 0579ae3d976091d7d664aa9d2527e0d0cff25763) # https://github.com/jbeder/yaml-cpp/releases
#FetchContent_MakeAvailable(yaml)
#
#FetchContent_Declare(zlib GIT_REPOSITORY https://github.com/intel/zlib.git
#        GIT_TAG 6160a8f20c3626aec2f8b0fda5bf2e65bfe31781) # https://github.com/curl/curl/releases
#FetchContent_MakeAvailable(zlib)
#
#FetchContent_Declare(curl GIT_REPOSITORY https://github.com/curl/curl.git
#        GIT_TAG 046209e561b7e9b5aab1aef7daebf29ee6e6e8c7) # https://github.com/curl/curl/releases
#FetchContent_MakeAvailable(curl)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 0817715923c9705e68994eb52ef9df3f6845beba) # The commit hash for 1.9.0. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

#target_link_libraries(freesight PRIVATE yaml-cpp zlibstatic libcurl project_options project_warnings)
target_link_libraries(freesight PRIVATE yaml-cpp libcurl cpr::cpr jsoncpp zip project_options project_warnings)

#if (WIN32)
#    add_custom_command(
#            TARGET freesight POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy_if_different
#            $<TARGET_RUNTIME_DLLS:freesight> $<TARGET_FILE_DIR:freesight>
#            COMMAND_EXPAND_LISTS
#    )
#endif ()

#include_directories( ${YAML_CPP_INCLUDE_DIR} )
#include_directories( ${CURL_INCLUDE_DIR} )
#include_directories( ${CPR_INCLUDE_DIRS} )
#include_directories( ${JSONCPP_INCLUDE_DIR} )
#include_directories( ${LIBZIP_INCLUDE_DIR} )

#include_directories( ${ZLIB_INCLUDE_DIRS} )

