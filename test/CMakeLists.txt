# https://cmake.org/cmake/help/v3.5/manual/ctest.1.html
# https://cmake.org/Wiki/CMake/Testing_With_CTest
# https://cmake.org/cmake/help/v3.5/command/add_test.html?

include(CTest)

# Create custom targets which executes CTEST and has
# dependecy of both binary files.
add_custom_target(check
    COMMAND echo [----] Running tests
    COMMAND ${CMAKE_CTEST_COMMAND}  --output-on-failure
    DEPENDS runningDict runningDictGui libRunningDict unittests)

add_custom_target(checkVerbose
    COMMAND echo [----] Running tests
    COMMAND ${CMAKE_CTEST_COMMAND}  -V --output-on-failure
    DEPENDS runningDict runningDictGui libRunningDict unittests)

#------------------------------------------------------------------------------


# Basic tests
add_test(NAME "Is_Runable "
    COMMAND $<TARGET_FILE:runningDict>)

add_test(NAME "Is_Runable-v"
    COMMAND $<TARGET_FILE:runningDict> -v)

add_test(NAME "Is_Runable-h"
    COMMAND $<TARGET_FILE:runningDict> -h)


#load tests
add_test(NAME "loads_dict"
    COMMAND $<TARGET_FILE:runningDict> -d test.dict
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)

add_test(NAME "loads_dict_prio"
    COMMAND $<TARGET_FILE:runningDict> -D test.dict
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)

add_test(NAME "loads_dict_2"
    COMMAND $<TARGET_FILE:runningDict> -d test.dict -d test2.dict
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)


#Do searching crash?
add_test(NAME "search_w1_d1"
    COMMAND $<TARGET_FILE:runningDict> -d test.dict rsu
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)

add_test(NAME "search_w2_d1"
    COMMAND $<TARGET_FILE:runningDict> -d test.dict rsu CMoglichIhre
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)

add_test(NAME "search_w2_d2"
    COMMAND $<TARGET_FILE:runningDict> -D test2.dict -d test.dict Katze rsu CMoglichIhre
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)


#Test searching with library
add_test(NAME "testLibrary"
    COMMAND python3 testLib.py $<TARGET_FILE:libRunningDict>
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)



# Add unit test to CTest suite as one of the tests
add_test(NAME "catch_unit_tests"
        COMMAND $<TARGET_FILE:unittests>
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
#------------------------------------------------------------------------------
# Unit tests

set(SRCTEST
    test.cpp
    ${PROJECT_BINARY_DIR}/source/version.cpp
    ${PROJECT_SOURCE_DIR}/source/Dict.cpp
    ${PROJECT_SOURCE_DIR}/source/Processer.cpp
    ${PROJECT_SOURCE_DIR}/source/Worker.cpp
    ${PROJECT_SOURCE_DIR}/source/log.cpp
    ${PROJECT_SOURCE_DIR}/source/gui/Logic.cpp
    )

add_executable(unittests EXCLUDE_FROM_ALL ${SRCTEST})
target_compile_definitions(unittests PRIVATE UNIT_TESTS)
set_property(TARGET unittests PROPERTY CXX_STANDARD 14) # we want C++14
add_dependencies(unittests Catch spdlog Json) # Catch, spdlog, Json is needed for unit tests, located in ../external
target_include_directories(unittests SYSTEM PRIVATE
    ${EXTERNAL_CATCH_INCLUDE_DIR}
    ${EXTERNAL_JSON_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/source
    ${PROJECT_SOURCE_DIR}/)

# convenience target for running only the unit tests
add_custom_target(unit
    #this way we can use faux data from /test dir (if we hady any):
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND $<TARGET_FILE:unittests> --use-colour yes
    DEPENDS unittests)
add_custom_target(unitall
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND $<TARGET_FILE:unittests> -s -a -r=compact --use-colour yes
    DEPENDS unittests)

