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

# add_library(COMMON_OBJS OBJECT ${SRCS})


find_package (Threads)
add_executable(runningDict main.cpp ${SRCS})
target_link_libraries (runningDict ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET runningDict PROPERTY CXX_STANDARD 14) # we want C++14

add_library(RunningDict SHARED LibInterface.cpp ${SRCS})
target_link_libraries (RunningDict ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET RunningDict PROPERTY CXX_STANDARD 14) # we want C++14


#copy of executables
install(TARGETS runningDict DESTINATION ./)
install(TARGETS RunningDict DESTINATION ./)


# no data needed
# install(DIRECTORY ${PROJECT_SOURCE_DIR}/data/ DESTINATION ./ )

#-------------------------------------------------------------------------------
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
