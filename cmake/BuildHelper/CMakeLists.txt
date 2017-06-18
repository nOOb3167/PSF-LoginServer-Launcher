# This CMakeLists.txt helps building a complete Launcher distribution.
#
# A complete Launcher distribution includes:
#   - Launcher executable built twice (32 bit and 64 bit).
#   - Installation of the Launcher into the local Maven repository.
#   - Deployment of the Launcher into a publicly accessible remote Maven repository via FTP.
#
# Launcher Installation and Deployment using Maven are optional and their targets must be manually invoked.
#   The way to do this depends on the CMake generator being used for the build.
#   (But basically 'make TARGETNAME' for Makefile-based generators, and
#    building from from IDE for Visual Studio-based generators)
#
# To perform the Install Maven step invoke the 'MavenInstall' CMake target.
# To perform the Deployment Maven step invoke the 'MavenDeploy' CMake target.
#
# The following variables can be customized:
#   CMAKE_INSTALL_PREFIX - Install location. While THIS CMakeLists.txt installs nothing, the variable
#                          is used when invoking other scripts.
#   CMAKE_GENERATOR      - This is set by:
#                            - https://cmake.org/cmake/help/v3.8/manual/cmake.1.html
#                              Invoking CMake with the -G <generator-name> option.
#                            - The CMake GUI application prompting for generator
#                              when selecting 'Configure'.
#   PSFLSL_GENERATOR     - Normally defaulted from CMAKE_GENERATOR.
#   PSFLSL_BUILDHELPER_BUILD_X32_DIR - Normally defaulted to cmake/Build_x32 within the LoginServer source tree.
#                                      Location where CMake will be invoked when building the 32-bit Launcher executable.
#   PSFLSL_BUILDHELPER_BUILD_X64_DIR - Normally defaulted to cmake/Build_x64 within the LoginServer source tree.
#                                      Location where CMake will be invoked when building the 64-bit Launcher executable.
#   PSFLSL_LOGINSERVER_SOURCE_DIR - Normally autodetected. BuildHelper is expected to be in the
#                                   cmake/BuildHelper/ directory within the LoginServer source tree.
#   PSFLSL_MAVEN_COMMAND          - Maven executable to invoke. Defaults to 'mvn', which should work if Maven was
#                                   put on the system PATH. Otherwise set to the full path.
#   PSFLSL_MAVEN_DEPLOY_URL       - FTP URL for Deployment.
#
# Example directory structure at CMAKE_INSTALL_PREFIX, after the install target has been built:
#   ${CMAKE_INSTALL_PREFIX}/bin/PSF-LoginServer-Launcher_x32.exe  - Launcher executable (32-bit)
#   ${CMAKE_INSTALL_PREFIX}/bin/PSF-LoginServer-Launcher_x32.pdb  - (Visual Studio debug file)
#   ${CMAKE_INSTALL_PREFIX}/bin/PSF-LoginServer-Launcher_x64.exe  - Launcher executable (64-bit)
#   ${CMAKE_INSTALL_PREFIX}/bin/PSF-LoginServer-Launcher_x64.pdb  - (Visual Studio debug file)
# If the 'MavenInstall' target has been invoked successfully, something like:
#   C:\Users\NAME\.m2\repository\net\psforever\launcher\PSF-LoginServer-Launcher_x32\VERSION\PSF-LoginServer-Launcher_x32-VERSION.exe
#   C:\Users\NAME\.m2\repository\net\psforever\launcher\PSF-LoginServer-Launcher_x64\VERSION\PSF-LoginServer-Launcher_x64-VERSION.exe
# (Maven Artifacts with coordinates 'net.psforever.launcher:PSF-LoginServer-Launcher_xBITNESS:VERSION' are installed)
# If the 'MavenDeploy' target has been invoked successfully, something like:
#   (Similar as for the 'MavenInstall' target, but uploaded to a FTP server)

CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

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
