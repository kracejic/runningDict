cmake_minimum_required (VERSION 3.0)
message(STATUS "")
message(STATUS "    == RunningDict configuration ==")
message(STATUS "")

# Project setup
project (RunningDict)
set(MAJOR_VERSION 0 CACHE STRING "major version" FORCE)
set(MINOR_VERSION 1 CACHE STRING "minor version" FORCE)
set(PATCH_VERSION 0 CACHE STRING "patch version" FORCE)
set(VERSION_TYPE "beta" CACHE STRING "version type" FORCE)
site_name(VERSION_HOST)
set(VERSION_HOST "${VERSION_HOST}" CACHE STRING "host of build" FORCE)

configure_file (
  "${PROJECT_SOURCE_DIR}/source/version.cpp.in"
  "${PROJECT_BINARY_DIR}/source/version.cpp"
  )


add_subdirectory(doc)

# Flags
option(PERF_TESTING "Turn ON to disable some compiler optimizations in order to ease analysis of performance" FALSE)
if(PERF_TESTING)
  set(CMAKE_CXX_FLAGS "-std=c++14 -O3 -Wall -fno-omit-frame-pointer -g -fno-rtti -fno-inline -fno-unroll-loops")
else()
  set(CMAKE_CXX_FLAGS "-std=c++14 -O3 -Wall -Wextra")
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/source
)

set(SRCS
    ${PROJECT_BINARY_DIR}/source/version.cpp
    source/Dict.cpp
    source/Worker.cpp
    source/Search.cpp
    source/Processer.cpp
)

# add_library(COMMON_OBJS OBJECT ${SRCS})


find_package (Threads)
add_executable(runMe source/main.cpp ${SRCS})
target_link_libraries (runMe ${CMAKE_THREAD_LIBS_INIT})

add_library(RunningDict SHARED source/LibInterface.cpp ${SRCS})
target_link_libraries (RunningDict ${CMAKE_THREAD_LIBS_INIT})


install(TARGETS runMe DESTINATION ./)
install(TARGETS RunningDict DESTINATION ./)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/data/
        DESTINATION ./
    )
install(DIRECTORY ${PROJECT_SOURCE_DIR}/test/
        DESTINATION ./
    )


# Copy MINGW libraries
if(MINGW)
    message(STATUS "MinGW detected")
    get_filename_component(GCC_PATH ${CMAKE_C_COMPILER} PATH)
    if(${GCC_PATH} MATCHES "mingw64/bin")
        set(libgcc "libgcc_s_seh-1.dll") #64bit
        set_target_properties(RunningDict PROPERTIES OUTPUT_NAME "RunningDict64")
        message(STATUS "  64bit dlls is building")
    else()
        set(libgcc "libgcc_s_dw2-1.dll") #32bit
        set_target_properties(RunningDict PROPERTIES OUTPUT_NAME "RunningDict32")
        message(STATUS "  32bit dlls is building")
    endif()

    install(FILES ${GCC_PATH}/${libgcc}
        ${GCC_PATH}/libwinpthread-1.dll
        ${GCC_PATH}/libstdc++-6.dll
        DESTINATION ./
    )
endif(MINGW)



if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist" CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
endif()

message(STATUS "")
message(STATUS "    == Final overview ==")
message(STATUS "Version:              ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION} ${VERSION_TYPE} @ ${VERSION_HOST}")
message(STATUS "Install prefix:       ${CMAKE_INSTALL_PREFIX}")
message(STATUS "PROJECT_SOURCE_DIR:   ${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_BUILD_TYPE:     ${CMAKE_BUILD_TYPE}")
message(STATUS "  possible options: Debug Release RelWithDebInfo MinSizeRel")
message(STATUS "  set with ` cmake -DCMAKE_BUILD_TYPE=Debug .. `")
message(STATUS "Building doc:         ${BUILD_DOCUMENTATION}")
message(STATUS "")



if(MINGW)
  # MESSAGE (STATUS "* Will copy MINGW libs to install folder: " ${GCC_PATH})
  MESSAGE (STATUS "* Will copy MINGW libs to install folder: ")
endif(MINGW)

message(STATUS "")
