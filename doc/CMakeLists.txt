find_package(Doxygen)

# Helper functions
macro(path_linux_to_win MsysPath ResultingPath)
  string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" ${ResultingPath} "${MsysPath}")
endmacro()
macro(path_win_to_linux MsysPath ResultingPath)
  string(REGEX REPLACE "^([a-zA-Z]):/" "/\\1/" ${ResultingPath} "${MsysPath}")
endmacro()

# enable if doxygen found
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    path_win_to_linux(${PROJECT_SOURCE_DIR} PROJECT_SOURCE_DIR_LINUX)
    path_win_to_linux(${PROJECT_BINARY_DIR} PROJECT_BINARY_DIR_LINUX)

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${doxyfile_in} ${doxyfile} @ONLY)
    add_custom_target(doc_build_only
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
    #install the test
    add_custom_target(doc
        COMMAND ${CMAKE_BUILD_TOOL} install
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS doc_build_only
        )



    install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/
        DESTINATION ./share/doc/${PROJECT_NAME})
endif()
