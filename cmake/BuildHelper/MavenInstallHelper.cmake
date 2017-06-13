IF (NOT CMAKE_INSTALL_PREFIX OR NOT PSFLSL_MAVEN_COMMAND OR NOT PSFLSL_FILE_DIR OR NOT PSFLSL_FILE_NAME_32 OR NOT PSFLSL_FILE_NAME_64)
  MESSAGE(FATAL_ERROR "May have been invoked without setting the necessary variables")
ENDIF ()

GET_FILENAME_COMPONENT(PSFLSL_ARTIFACTID_32 "${PSFLSL_FILE_NAME_32}" NAME_WE)
GET_FILENAME_COMPONENT(PSFLSL_ARTIFACTID_64 "${PSFLSL_FILE_NAME_64}" NAME_WE)

SET(PSFLSL_FILE_32 "${PSFLSL_FILE_DIR}/${PSFLSL_FILE_NAME_32}")
SET(PSFLSL_FILE_64 "${PSFLSL_FILE_DIR}/${PSFLSL_FILE_NAME_64}")

IF (NOT (EXISTS "${PSFLSL_FILE_32}") OR NOT (EXISTS "${PSFLSL_FILE_64}"))
  MESSAGE(FATAL_ERROR "32 or 64 bit Launcher executable not found"
                       "- an install may not have successfully run")
ENDIF ()

EXECUTE_PROCESS(
  COMMAND
    "${PSFLSL_MAVEN_COMMAND}"
    "org.apache.maven.plugins:maven-install-plugin:2.5.2:install-file"
    "-Dfile=${PSFLSL_FILE_32}"
    "-DgroupId=net.psforever.launcher"
    "-DartifactId=${PSFLSL_ARTIFACTID_32}"
    "-Dversion=NOVERSION"
    "-Dpackaging=exe"
  COMMAND
    "${PSFLSL_MAVEN_COMMAND}"
    "org.apache.maven.plugins:maven-install-plugin:2.5.2:install-file"
    "-Dfile=${PSFLSL_FILE_64}"
    "-DgroupId=net.psforever.launcher"
    "-DartifactId=${PSFLSL_ARTIFACTID_64}"
    "-Dversion=NOVERSION"
    "-Dpackaging=exe"
  RESULT_VARIABLE
    TMP_RETCODE
  OUTPUT_VARIABLE
    TMP_STREAM
  ERROR_VARIABLE
    TMP_STREAM
)

IF (NOT "${TMP_RETCODE}" STREQUAL "0")
  MESSAGE("Maven Executable (NOTE: may need full path!): ${PSFLSL_MAVEN_COMMAND}")
  MESSAGE("Error Code: ${TMP_RETCODE}")
  MESSAGE("(BEGIN MAVEN STDOUT STDERR DUMP)")
  MESSAGE("${TMP_STREAM}")
  MESSAGE("(END   MAVEN STDOUT STDERR DUMP)")
  MESSAGE(FATAL_ERROR "Error Invoking Maven")
ENDIF ()
