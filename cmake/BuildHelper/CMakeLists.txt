CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
# CMake "-A" option (Useful when invoking CMAKE_COMMAND)
#   Selects a generator platform.
#   https://cmake.org/cmake/help/v3.9/generator/Visual%20Studio%2012%202013.html
#     Can't find an exhaustive list of valid platforms but:
#       -A option not used at all = 32 bit
#       -A x64 option = 64 bit

PROJECT (PSFLSL-LoginServer-Launcher-BuildHelper
  LANGUAGES NONE)

GET_FILENAME_COMPONENT(PSFLSL_LOGINSERVER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE CACHE)

SET(PSFLSL_BUILDHELPER_BUILD_X32_DIR "${PSFLSL_LOGINSERVER_SOURCE_DIR}/cmake/Build_x32/" CACHE PATH "Build directory (x32)")
SET(PSFLSL_BUILDHELPER_BUILD_X64_DIR "${PSFLSL_LOGINSERVER_SOURCE_DIR}/cmake/Build_x64/" CACHE PATH "Build directory (x64)")

SET(PSFLSL_GENERATOR "${CMAKE_GENERATOR}" CACHE STRING "(Generator (Visual Studio) - Will be stripped of any platform suffix)")

SET(PSFLSL_MAVEN_COMMAND "mvn" CACHE FILEPATH "Maven Command (Used when invoking the Maven-specific targets)")
SET(PSFLSL_MAVEN_DEPLOY_URL "ftp://kazimir1.ddns.net/home/ftpuser1/repo" CACHE STRING "(Used when invoking the Maven-specific targets)")

IF (PSFLSL_GENERATOR MATCHES "^Visual Studio")
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/BuildMsvc.cmake")
ELSE ()
  MESSAGE(FATAL_ERROR "BuildHelper not designed for used platform?")
ENDIF ()

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/BuildMaven.cmake")
