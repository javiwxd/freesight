cmake_minimum_required(VERSION 3.24)
set(CMAKE_GENERATOR_PLATFORM x64)
include(FetchContent)

project(freesight)

set(CMAKE_CXX_STANDARD 23)
option(CURL_USE_SCHANNEL "Use windows default SSL libraries" ON)
option(CMAKE_USE_WINSSL "Set to ON to use WINSSL for windows." ON)

add_executable(freesight main.cpp targetver.h stdafx.cpp stdafx.h firewall/windows_firewall.cpp firewall/windows_firewall.h logger/logger.h logger/color.h logger/banner.h config/config.h lib/split.h lib/elevate.h res/metadata.rc vpn/installer.h http/manager.cpp http/manager.h regedit/regedit.h regedit/regedit.cpp)
target_precompile_headers(freesight PUBLIC stdafx.h)

FetchContent_Declare(yaml GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
        GIT_TAG 0579ae3d976091d7d664aa9d2527e0d0cff25763) # https://github.com/jbeder/yaml-cpp/releases
FetchContent_MakeAvailable(yaml)

FetchContent_Declare(curl GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG 046209e561b7e9b5aab1aef7daebf29ee6e6e8c7) # https://github.com/curl/curl/releases
FetchContent_MakeAvailable(curl)

target_link_libraries(freesight PRIVATE yaml-cpp libcurl)

include_directories( ${YAML_CPP_INCLUDE_DIR} )
include_directories( ${CURL_INCLUDE_DIR} )

