configure_file (
  "${PROJECT_SOURCE_DIR}/source/version.cpp.in"
  "${PROJECT_BINARY_DIR}/source/version.cpp"
  )

# Flags
if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/source
    ${PROJECT_SOURCE_DIR}/
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
install(TARGETS runningDict DESTINATION ./)
install(TARGETS libRunningDict DESTINATION ./)



# install all data
install(DIRECTORY ${PROJECT_SOURCE_DIR}/data/ DESTINATION ./ )

if(MINGW)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/dataWindows/ DESTINATION ./ )
endif(MINGW)

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
        DESTINATION ./
    )
endif(MINGW)



#-------------------------------------------------------------------------------
# RunningDict2Gui

set(GUISRC
    gui/guiMain.cpp
    gui/MainWindow.cpp
    gui/SettingsWindow.cpp
    gui/NewWordWindow.cpp
    gui/Logic.cpp
)

find_package(PkgConfig)
pkg_check_modules(GTK3MM gtkmm-3.0)

if(GTK3MM_FOUND)
    message(STATUS "GTK was found, building GUI")

    #executable setup WIN32 forces no command line window
    if(MINGW)
        add_executable(runningDictGui WIN32 ${GUISRC} ${SRCS})
    else()
        add_executable(runningDictGui ${GUISRC} ${SRCS})
    endif()

    target_include_directories(runningDictGui PUBLIC ${GTK3MM_INCLUDE_DIRS})
    target_link_libraries (runningDictGui
        ${CMAKE_THREAD_LIBS_INIT} ${GTK3MM_LIBRARIES})
    set_property(TARGET runningDictGui PROPERTY CXX_STANDARD 14) # we want C++14

    install(TARGETS runningDictGui DESTINATION ./)

    #windows MSYS2 support
    macro(installMSYS2lib FilePattern)
        install(DIRECTORY ${GCC_PATH}/ DESTINATION ./
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
    message(STATUS "GTKMM-3 was not found, will not build GUI component")
endif()


# Static analysis via clang-tidy
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp)
add_custom_target(
        analyze
        COMMAND clang-tidy
        ${ALL_SOURCE_FILES}
        -p=./
)