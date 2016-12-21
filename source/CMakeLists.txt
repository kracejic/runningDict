configure_file (
  "${PROJECT_SOURCE_DIR}/source/version.cpp.in"
  "${PROJECT_BINARY_DIR}/source/version.cpp"
  )

# Flags
if(NOT MSVC)
    add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wunused -pedantic)
    #-Wmisleading-indentation 
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/source
)

set(SRCS
    ${PROJECT_BINARY_DIR}/source/version.cpp
    Dict.cpp
    Worker.cpp
    Search.cpp
    Processer.cpp
)

set(HEADERS
    version.h
    Dict.h
    Worker.h
    Search.h
    Processer.h
    SpeedTimer.h
)

# add_library(COMMON_OBJS OBJECT ${SRCS})


find_package (Threads)
add_executable(runningDict main.cpp ${SRCS} ${HEADERS})
target_link_libraries (runningDict ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET runningDict PROPERTY CXX_STANDARD 14) # we want C++14

add_library(libRunningDict SHARED LibInterface.cpp ${SRCS} ${HEADERS} LibInterface.h)
target_link_libraries (libRunningDict ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET libRunningDict PROPERTY CXX_STANDARD 14) # we want C++14



#copy of executables
install(TARGETS runningDict DESTINATION bin)
install(TARGETS libRunningDict DESTINATION bin)




#-------------------------------------------------------------------------------
# Copy MINGW libraries
if(MINGW)
    message(STATUS "MinGW detected")
    get_filename_component(GCC_PATH ${CMAKE_C_COMPILER} PATH)
    if(${GCC_PATH} MATCHES "mingw64/bin")
        set(libgcc "libgcc_s_seh-1.dll") #64bit
        set_target_properties(libRunningDict PROPERTIES OUTPUT_NAME "RunningDict64")
        message(STATUS "  64bit dlls is building")
    else()
        set(libgcc "libgcc_s_dw2-1.dll") #32bit
        set_target_properties(libRunningDict PROPERTIES OUTPUT_NAME "RunningDict32")
        message(STATUS "  32bit dlls is building")
    endif()

    install(FILES ${GCC_PATH}/${libgcc}
        ${GCC_PATH}/libwinpthread-1.dll
        ${GCC_PATH}/libstdc++-6.dll
        DESTINATION bin
    )
endif(MINGW)



#-------------------------------------------------------------------------------
# RunningDict2Gui

set(GUISRC
    gui/guiMain.cpp
    gui/MainWindow.cpp
    gui/SettingsWindow.cpp
    gui/NewWordWindow.cpp
    gui/ChangeWordWindow.cpp
    gui/Logic.cpp
)

find_package(PkgConfig)
pkg_check_modules(GTK3MM gtkmm-3.0)

if(GTK3MM_FOUND)
    message(STATUS "Standalone gui part                       YES ")
    if(WIN32) #windows icon resource file, provided from /packaging directory
        set(WINDOWS_ICON_RESOURCE "${PROJECT_BINARY_DIR}/exampleApp.icon.rc")
    endif()


    #executable setup WIN32 forces no command line window
    if(MINGW)
        add_executable(runningDictGui WIN32 ${GUISRC} ${SRCS} ${WINDOWS_ICON_RESOURCE})
    else()
        add_executable(runningDictGui ${GUISRC} ${SRCS} ${WINDOWS_ICON_RESOURCE})
    endif()
    add_dependencies(runningDictGui Json)

    add_custom_target(gui-run
	WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
	COMMAND ./runningDictGui
	DEPENDS runningDictGui Install_)
    add_custom_target(run DEPENDS gui-run)


    # convenience run gdb target
    find_program(GDB_PATH gdb)
    if(GDB_PATH)
        add_custom_target(gdb
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
            COMMAND ${GDB_PATH} ./runningDictGui
            DEPENDS runningDictGui Install_)
    endif()
    

    target_include_directories(runningDictGui SYSTEM PRIVATE 
        ${GTK3MM_INCLUDE_DIRS} 
        ${EXTERNAL_JSON_INCLUDE_DIR})
    target_link_libraries (runningDictGui
        ${CMAKE_THREAD_LIBS_INIT} ${GTK3MM_LIBRARIES})
    set_property(TARGET runningDictGui PROPERTY CXX_STANDARD 14) # we want C++14

    install(TARGETS runningDictGui DESTINATION bin)

    #windows MSYS2 support
    macro(installMSYS2lib FilePattern)
        install(DIRECTORY ${GCC_PATH}/ DESTINATION bin
            FILES_MATCHING PATTERN ${FilePattern})
    endmacro()
    if(MINGW)
        installMSYS2lib("libatk-1.0-0.dll")
        installMSYS2lib("libatkmm-1.6-1.dll")
        installMSYS2lib("libbz2-1.dll")
        installMSYS2lib("libcairo-2.dll")
        installMSYS2lib("libcairo-gobject-2.dll")
        installMSYS2lib("libcairomm-1.0-1.dll")
        installMSYS2lib("libepoxy-0.dll")
        installMSYS2lib("libexpat-1.dll")
        installMSYS2lib("libffi-6.dll")
        installMSYS2lib("libfontconfig-1.dll")
        installMSYS2lib("libfreetype-6.dll")
        installMSYS2lib("libgdk_pixbuf-2.0-0.dll")
        installMSYS2lib("libgdk-3-0.dll")
        installMSYS2lib("libgdkmm-3.0-1.dll")
        installMSYS2lib("libgio-2.0-0.dll")
        installMSYS2lib("libgiomm-2.4-1.dll")
        installMSYS2lib("libglib-2.0-0.dll")
        installMSYS2lib("libglibmm-2.4-1.dll")
        installMSYS2lib("libgmodule-2.0-0.dll")
        installMSYS2lib("libgobject-2.0-0.dll")
        installMSYS2lib("libgraphite2.dll")
        installMSYS2lib("libgtk-3-0.dll")
        installMSYS2lib("libgtkmm-3.0-1.dll")
        installMSYS2lib("libharfbuzz-0.dll")
        installMSYS2lib("libiconv-2.dll")
        installMSYS2lib("libintl-8.dll")
        installMSYS2lib("libpango-1.0-0.dll")
        installMSYS2lib("libpangocairo-1.0-0.dll")
        installMSYS2lib("libpangoft2-1.0-0.dll")
        installMSYS2lib("libpangomm-1.4-1.dll")
        installMSYS2lib("libpangowin32-1.0-0.dll")
        installMSYS2lib("libpcre-1.dll")
        installMSYS2lib("libpixman-1-0.dll")
        installMSYS2lib("libpng16-16.dll")
        installMSYS2lib("libsigc-2.0-0.dll")
        installMSYS2lib("zlib1.dll")
    endif(MINGW)

    #Boost instead of C++17 filesystem (MSYS2 GCC5.3 does not have filesystem TS)
    option(USE_BOOST_FILESYSTEM "whether to use boost::filesystem instead of the experimental filesystem"
        FALSE)
    if(USE_BOOST_FILESYSTEM)
        find_package(Boost COMPONENTS filesystem system REQUIRED)
        target_compile_definitions(runningDictGui
            PRIVATE USE_BOOST_FILESYSTEM)

        target_include_directories(runningDictGui
            SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
        target_link_libraries(runningDictGui ${Boost_FILESYSTEM_LIBRARY}
            ${Boost_SYSTEM_LIBRARY})
        #TODO install boost dll on windows
        installMSYS2lib("libboost_filesystem-mt.dll")
        installMSYS2lib("libboost_system-mt.dll")
    else()
        target_link_libraries(runningDictGui  stdc++fs)
    endif()

else()
    message(STATUS "Standalone gui part (no Gtkmm3)           NO ")
endif()



#------------------------------------------------------------------------------
# Other targets

file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.cc *.c)
file(GLOB_RECURSE ALL_HEADER_FILES *.h *.hpp)

# Static analysis via clang-tidy target
# We check for program, since when it is not here, target makes no sense
find_program(TIDY_PATH clang-tidy PATHS /usr/local/Cellar/llvm/*/bin)
if(TIDY_PATH)
    message(STATUS "clang-tidy - static analysis              YES ")
    add_custom_target(tidy
        COMMAND ${TIDY_PATH} ${ALL_SOURCE_FILES} -p=./ )
else()
    message(STATUS "clang-tidy - static analysis              NO ")
endif()

# run clang-format on all files
find_program(FORMAT_PATH clang-format)
if(FORMAT_PATH)
    message(STATUS "clang-format - code formating             YES ")
    add_custom_target(format
        COMMAND ${FORMAT_PATH} -i ${ALL_SOURCE_FILES} ${ALL_HEADER_FILES} )
else()
    message(STATUS "clang-format - code formating             NO ")
endif()

# cpp check static analysis
find_program(CPPCHECK_PATH cppcheck)
if(CPPCHECK_PATH)
    message(STATUS "cppcheck - static analysis                YES ")
    add_custom_target(
            cppcheck
            COMMAND ${CPPCHECK_PATH}
            --enable=warning,performance,portability,information,missingInclude
            --std=c++11
            --template=gcc
            --verbose
            --quiet
            ${ALL_SOURCE_FILES} ${ALL_HEADER_FILES}
    )
else()
    message(STATUS "cppcheck - static analysis                NO ")
endif()


# Does not work well, left here for future work, but it would still only
# provide same info as tidy, only in html form.
#
# Produces html analysis in *.plist dirs in build dir or build/source directory
# add_custom_target(
#     analyze
#     COMMAND rm -rf ../*.plist
#     COMMAND rm -rf *.plist
#     COMMAND clang-check -analyze -extra-arg -Xclang -extra-arg -analyzer-output=html
#     ${ALL_SOURCE_FILES}
#     -p=./
#     COMMAND echo ""
#     )

