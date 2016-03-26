# https://cmake.org/cmake/help/v3.5/manual/ctest.1.html
# https://cmake.org/Wiki/CMake/Testing_With_CTest
# https://cmake.org/cmake/help/v3.5/command/add_test.html?

include(CTest)


enable_testing()
add_test(NAME "Is_Runable " COMMAND $<TARGET_FILE:runningDict>)
add_test(NAME "Is_Runable-v" COMMAND $<TARGET_FILE:runningDict> -v)
add_test(NAME "Is_Runable-h" COMMAND $<TARGET_FILE:runningDict> -h)

add_test(NAME "loads_dict" COMMAND $<TARGET_FILE:runningDict> -d test.dict WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)
add_test(NAME "loads_dict_prio" COMMAND $<TARGET_FILE:runningDict> -D test.dict WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)
add_test(NAME "loads_dict_2" COMMAND $<TARGET_FILE:runningDict> -d test.dict -d test2.dict WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)

add_test(NAME "search_w1_d1" COMMAND $<TARGET_FILE:runningDict> -d test.dict rsu WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)
add_test(NAME "search_w2_d1" COMMAND $<TARGET_FILE:runningDict> -d test.dict rsu CMoglichIhre WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)
add_test(NAME "search_w2_d2" COMMAND $<TARGET_FILE:runningDict> -D test2.dict -d test.dict Katze rsu CMoglichIhre WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)

add_test(NAME "testLibrary" COMMAND python3 testLib.py $<TARGET_FILE:RunningDict>
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)
