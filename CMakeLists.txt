cmake_minimum_required (VERSION 3.0)
project (RunningDict)
message(STATUS "")
message(STATUS "    == ${PROJECT_NAME} configuration ==")
message(STATUS "")

# Project setup
set(MAJOR_VERSION 1 CACHE STRING "major version" FORCE)
set(MINOR_VERSION 0 CACHE STRING "minor version" FORCE)
set(PATCH_VERSION 1 CACHE STRING "patch version" FORCE)
set(VERSION_TYPE "beta" CACHE STRING "version type" FORCE)
site_name(VERSION_HOST)
set(VERSION_HOST "${VERSION_HOST}" CACHE STRING "host of build" FORCE)

#be nice and export compile commands by default
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()

#------------------------------------------------------------------------------
# set default install path
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist" CACHE PATH 
        "Install path prefix, prepended onto install directories." FORCE )
endif()
#------------------------------------------------------------------------------
# Custom Install target, used in run target in source/CMakeLists.txt
add_custom_target( Install_ COMMAND "${CMAKE_COMMAND}" --build . --target install
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

#------------------------------------------------------------------------------
# Included files

add_subdirectory(doc)
add_subdirectory(external)


add_subdirectory(test/sanitizers)
add_subdirectory(test)
add_subdirectory(source)







#------------------------------------------------------------------------------
# sum up build info
message(STATUS "")
message(STATUS "    == Final overview ==")
message(STATUS "Version:              ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION} ${VERSION_TYPE} @ ${VERSION_HOST}")
message(STATUS "Install prefix:       ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_BUILD_TYPE:     ${CMAKE_BUILD_TYPE}")
message(STATUS "  possible options: Debug Release RelWithDebInfo MinSizeRel")
message(STATUS "  set with ` cmake -DCMAKE_BUILD_TYPE=Debug .. `")

if(MINGW)
  # MESSAGE (STATUS "* Will copy MINGW libs to install folder: " ${GCC_PATH})
  MESSAGE (STATUS "* Will copy MINGW libs to install folder: ")
endif(MINGW)

message(STATUS "")
