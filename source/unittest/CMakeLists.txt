
#------------------------------------------------------------------------------
# Unit tests
set(SRCTEST
    test.cpp
    cpr.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/../version.cpp
    ../Dict.cpp
    ../Processer.cpp
    ../Worker.cpp
    ../log.cpp
    ../gui/Logic.cpp
    )

add_executable(unittests EXCLUDE_FROM_ALL ${SRCTEST})
target_compile_definitions(unittests PRIVATE UNIT_TESTS)
set_property(TARGET unittests PROPERTY CXX_STANDARD 14) # we want C++14
add_dependencies(unittests Catch spdlog Json Cpr-ext) # Catch, spdlog, Json is needed for unit tests, located in ../external
target_include_directories(unittests SYSTEM PRIVATE
    ${EXTERNAL_CATCH_INCLUDE_DIR}
    ${EXTERNAL_JSON_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/source
    ${PROJECT_SOURCE_DIR}/
    ${CPR_INCLUDE_DIRS})
target_link_libraries(unittests ${CPR_LIBRARIES})
