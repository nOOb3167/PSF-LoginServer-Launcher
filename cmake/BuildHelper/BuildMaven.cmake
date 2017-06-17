# - Define targets (Maven specific custom targets for manual invocation)

IF (NOT DEFINED CMAKE_INSTALL_PREFIX             OR
    NOT DEFINED PSFLSL_LOGINSERVER_SOURCE_DIR    OR
    NOT DEFINED PSFLSL_MAVEN_COMMAND             OR
    NOT DEFINED PSFLSL_MAVEN_DEPLOY_URL)
  MESSAGE(FATAL_ERROR "May have been INCLUDEd without setting the necessary variables")
ENDIF ()

# Prior to invoking these targets, the install prefix directory (CMAKE_INSTALL_PREFIX)
# must have been populated. To do this, invoke the configure and build targets
# defined elsewhere by BuildHelper.
# The maven targets defined here currently are NOT setup to depend on the configure and/or build
# targets (and thus to invoke them automatically).

# This script must 'know' the layout of the install prefix directory.
# The layout is formed basically by the INSTALL commands withing the 'parent' PSFLSL CMakeLists)
#   Example: INSTALL(TARGETS PSF-LoginServer-Launcher ... RUNTIME DESTINATION "bin" ... )
# Getting the executable name right, specifically, is important here.

SET(PSFLSL_FILE_DIR "${CMAKE_INSTALL_PREFIX}/bin/")
SET(PSFLSL_FILE_NAME_32 "PSF-LoginServer-Launcher_x32.exe")
SET(PSFLSL_FILE_NAME_64 "PSF-LoginServer-Launcher_x64.exe")

ADD_CUSTOM_TARGET(MavenInstall
  COMMAND
    "${CMAKE_COMMAND}"
      "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
      "-DPSFLSL_MAVEN_COMMAND=${PSFLSL_MAVEN_COMMAND}"
      "-DPSFLSL_FILE_DIR=${PSFLSL_FILE_DIR}"
      "-DPSFLSL_FILE_NAME_32=${PSFLSL_FILE_NAME_32}"
      "-DPSFLSL_FILE_NAME_64=${PSFLSL_FILE_NAME_64}"
      -P "${PSFLSL_LOGINSERVER_SOURCE_DIR}/cmake/BuildHelper/MavenInstallHelper.cmake"
  COMMENT
    "Invoking MavenInstallHelper"
)

ADD_CUSTOM_TARGET(MavenDeploy
  COMMAND
    "${CMAKE_COMMAND}"
      "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
      "-DPSFLSL_MAVEN_COMMAND=${PSFLSL_MAVEN_COMMAND}"
      "-DPSFLSL_FILE_DIR=${PSFLSL_FILE_DIR}"
      "-DPSFLSL_FILE_NAME_32=${PSFLSL_FILE_NAME_32}"
      "-DPSFLSL_FILE_NAME_64=${PSFLSL_FILE_NAME_64}"
      "-DPSFLSL_MAVEN_DEPLOY_HELPER_POM_PATH=${PSFLSL_LOGINSERVER_SOURCE_DIR}/cmake/BuildHelper/MavenDeployHelperPom.xml"
      "-DPSFLSL_MAVEN_DEPLOY_URL=${PSFLSL_MAVEN_DEPLOY_URL}"
      -P "${PSFLSL_LOGINSERVER_SOURCE_DIR}/cmake/BuildHelper/MavenDeployHelper.cmake"
  COMMENT
    "Invoking MavenDeployHelper"
)
