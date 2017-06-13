# check required variables were defined when executing this file

IF (NOT CMAKE_INSTALL_PREFIX)
  MESSAGE(FATAL_ERROR "MavenInstallHelper Missing CMAKE_INSTALL_PREFIX")
ENDIF ()

IF (NOT PSFLSL_MAVEN_COMMAND)
  MESSAGE(FATAL_ERROR "MavenInstallHelper Missing PSFLSL_MAVEN_COMMAND")
ENDIF ()

# check required target export files are present

SET(PSFLSL_EXPORTED_FILENAME_32 "${CMAKE_INSTALL_PREFIX}/cmake/x32-psflsl-executable-export.cmake")
SET(PSFLSL_EXPORTED_FILENAME_64 "${CMAKE_INSTALL_PREFIX}/cmake/x64-psflsl-executable-export.cmake")

IF (NOT (EXISTS "${PSFLSL_EXPORTED_FILENAME_32}"))
  MESSAGE(FATAL_ERROR "MavenInstallHelper Missing File (PSFLSL_EXPORTED_FILENAME_32)")
ENDIF ()

IF (NOT (EXISTS "${PSFLSL_EXPORTED_FILENAME_64}"))
  MESSAGE(FATAL_ERROR "MavenInstallHelper Missing File (PSFLSL_EXPORTED_FILENAME_64)")
ENDIF ()

# import target export files

INCLUDE("${PSFLSL_EXPORTED_FILENAME_32}")
INCLUDE("${PSFLSL_EXPORTED_FILENAME_64}")

# run maven install plugin

## artifactId are the executable filenames without extension

SET(PSFLSL_FILE_32 "$<TARGET_FILE_NAME:x32-PSF-LoginServer-Launcher>")
SET(PSFLSL_FILE_64 "$<TARGET_FILE_NAME:x64-PSF-LoginServer-Launcher>")

GET_FILENAME_COMPONENT(PSFLSL_ARTIFACTID_32 "${PSFLSL_FILE_32}" NAME_WE)
GET_FILENAME_COMPONENT(PSFLSL_ARTIFACTID_64 "${PSFLSL_FILE_64}" NAME_WE)

EXECUTE_PROCESS(
  COMMAND
    "${PSFLSL_MAVEN_COMMAND}"
    "org.apache.maven.plugins:maven-install-plugin:2.5.2:install-file"
    -Dfile="${PSFLSL_FILE_32}"
    -DgroupId="net.psforever.launcher"
    -DartifactId="${PSFLSL_ARTIFACTID_32}"
    -Dversion="0.1"
    -Dpackaging="exe"
  COMMAND
    "${PSFLSL_MAVEN_COMMAND}"
    "org.apache.maven.plugins:maven-install-plugin:2.5.2:install-file"
    -Dfile="${PSFLSL_FILE_64}"
    -DgroupId="net.psforever.launcher"
    -DartifactId="${PSFLSL_ARTIFACTID_64}"
    -Dversion="0.1"
    -Dpackaging="exe"
)
