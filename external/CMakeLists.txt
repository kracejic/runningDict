include(ExternalProject)
find_package(Git REQUIRED)
# find_package(Hg REQUIRED)


#------------------------------------------------------------------------------
# Catch - Unit test header only library
#  * sets EXTERNAL_CATCH_INCLUDE_DIR 

ExternalProject_Add(
    Catch
    PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/Catch
    GIT_REPOSITORY https://github.com/philsquared/Catch.git
    # For shallow git clone (without downloading whole history)
    # GIT_SHALLOW 1
    # For point at certain tag
    # GIT_TAG 1.5.7
    #disables auto update on every build
    UPDATE_DISCONNECTED 1
    #disable following
    CONFIGURE_COMMAND "" BUILD_COMMAND "" INSTALL_DIR "" INSTALL_COMMAND ""
    )
# Update Catch target
add_custom_target(external-Catch-update
    COMMENT "Updated Catch"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Catch/src/Catch
    COMMAND ${GIT_EXECUTABLE} pull
    DEPENDS Catch)
# set include directory
set(EXTERNAL_CATCH_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Catch/src/Catch/single_include" PARENT_SCOPE)


#------------------------------------------------------------------------------
# nlohmann/json
#  * sets EXTERNAL_JSON_INCLUDE_DIR 

ExternalProject_Add(
    Json
    PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/Json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    # For shallow git clone (without downloading whole history)
    # GIT_SHALLOW 1
    # For point at certain tag
    # GIT_TAG 1.5.7
    #disables auto update on every build
    UPDATE_DISCONNECTED 1
    #disable following
    CONFIGURE_COMMAND "" BUILD_COMMAND "" INSTALL_DIR "" INSTALL_COMMAND ""
    )
# Update json target
add_custom_target(external-Json-update
    COMMENT "Updated Nlohmann/Json"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Json/src/Json
    COMMAND ${GIT_EXECUTABLE} pull
    DEPENDS Json)
# set include directory
set(EXTERNAL_JSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Json/src/Json/src" PARENT_SCOPE)


#------------------------------------------------------------------------------
# spdlog
#  * sets EXTERNAL_SPDLOG_INCLUDE_DIR 

ExternalProject_Add(
    spdlog
    PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    # For shallow git clone (without downloading whole history)
    # GIT_SHALLOW 1
    # For point at certain tag
    # GIT_TAG 1.5.7
    #disables auto update on every build
    UPDATE_DISCONNECTED 1
    #disable following
    CONFIGURE_COMMAND "" BUILD_COMMAND "" INSTALL_DIR "" INSTALL_COMMAND ""
    )
# Update json target
add_custom_target(external-spdlog-update
    COMMENT "Updated gabime/spdlog"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/src/spdlog
    COMMAND ${GIT_EXECUTABLE} pull
    DEPENDS spdlog)
# set include directory
set(EXTERNAL_SPDLOG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/spdlog/src/spdlog/include" PARENT_SCOPE)


#------------------------------------------------------------------------------
# Convenience update all
add_custom_target(external-update-all
    COMMENT "Updated all external projects"
    DEPENDS external-Catch-update external-Json-update external-spdlog-update)

