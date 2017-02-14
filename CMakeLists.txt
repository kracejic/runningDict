cmake_minimum_required (VERSION 3.0)
message(STATUS "")
message(STATUS "    == ${PROJECT_NAME} configuration ==")
message(STATUS "")

# Project setup
project (runningdict VERSION 1.0.3)
set(VERSION_TYPE "beta" CACHE STRING "version type" FORCE)
site_name(VERSION_HOST)
set(VERSION_HOST "${VERSION_HOST}" CACHE STRING "host of build" FORCE)

# Be nice to visual studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#be nice and export compile commands by default
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#------------------------------------------------------------------------------
# set default install path
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist" CACHE PATH
        "Install path prefix, prepended onto install directories." FORCE )
endif()
#------------------------------------------------------------------------------
# Custom Install target, used in run target in source/CMakeLists.txt
if (CMAKE_GENERATOR MATCHES "Makefiles")
    # Make it multithreaded
    add_custom_target( Install_ COMMAND "${CMAKE_COMMAND}" --build . --target
        install -- -j WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
else()
    add_custom_target( Install_ COMMAND "${CMAKE_COMMAND}" --build . --target
        install WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()


#------------------------------------------------------------------------------
# Dependencies for all targets

# Boost instead of C++17 filesystem (MSYS2 GCC5.3 does not have filesystem TS)
option(USE_BOOST_FILESYSTEM 
    "whether to use boost::filesystem instead of the std::filesystem" FALSE)
if(USE_BOOST_FILESYSTEM)
    find_package(Boost COMPONENTS filesystem system REQUIRED)
    add_definitions("-DUSE_BOOST_FILESYSTEM")
    include_directories( SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
    link_libraries(${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
else()
    link_libraries(stdc++fs)
endif()


#------------------------------------------------------------------------------
# Included files


# Documentation build
add_subdirectory(doc)

# External resources/repositories are downloaded here
add_subdirectory(external)

# Images, databases and other data which needs to be installed for project
add_subdirectory(data)

# Testing
enable_testing()
add_subdirectory(test)

# Source code
add_subdirectory(source)

# Packaging stuff (deb, rpm, windows installer)
add_subdirectory(packaging)





#------------------------------------------------------------------------------
# sum up build info
message(STATUS "")
message(STATUS "    == Final overview ==")
message(STATUS "Version:              ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} ${VERSION_TYPE} @ ${VERSION_HOST}")
message(STATUS "Install prefix:       ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_BUILD_TYPE:     ${CMAKE_BUILD_TYPE}")
message(STATUS "  possible options: Debug Release RelWithDebInfo MinSizeRel")
message(STATUS "  set with ` cmake -DCMAKE_BUILD_TYPE=Debug .. `")

if(MINGW)
  # MESSAGE (STATUS "* Will copy MINGW libs to install folder: " ${GCC_PATH})
  MESSAGE (STATUS "* Will copy MINGW libs to install folder: ")
endif(MINGW)

message(STATUS "")
