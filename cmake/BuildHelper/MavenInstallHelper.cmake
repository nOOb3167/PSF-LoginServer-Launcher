IF (NOT DEFINED CMAKE_INSTALL_PREFIX OR
    NOT DEFINED PSFLSL_MAVEN_COMMAND OR
    NOT DEFINED PSFLSL_FILE_DIR      OR
    NOT DEFINED PSFLSL_FILE_NAME_32  OR
    NOT DEFINED PSFLSL_FILE_NAME_64)
  MESSAGE(FATAL_ERROR "May have been invoked without setting the necessary variables")
ENDIF ()

# Name of the file being installed into the maven repository, sans extension (eg. exe),
# is used as the Maven artifactId.
# This artifactId (together with groupId and version) can be used in a <dependency>
# element withing a Maven pom.xml file.

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
  RESULT_VARIABLE
    TMP_RETCODE1
  OUTPUT_VARIABLE
    TMP_STREAM1
  ERROR_VARIABLE
    TMP_STREAM1
)

EXECUTE_PROCESS(
  COMMAND
    "${PSFLSL_MAVEN_COMMAND}"
    "org.apache.maven.plugins:maven-install-plugin:2.5.2:install-file"
    "-Dfile=${PSFLSL_FILE_64}"
    "-DgroupId=net.psforever.launcher"
    "-DartifactId=${PSFLSL_ARTIFACTID_64}"
    "-Dversion=NOVERSION"
    "-Dpackaging=exe"
  RESULT_VARIABLE
    TMP_RETCODE2
  OUTPUT_VARIABLE
    TMP_STREAM2
  ERROR_VARIABLE
    TMP_STREAM2
)

MESSAGE("(BEGIN MAVEN STDOUT STDERR DUMP)")
MESSAGE("====FIRST=====")
MESSAGE("${TMP_STREAM1}")
MESSAGE("====SECOND====")
MESSAGE("${TMP_STREAM2}")
MESSAGE("(END   MAVEN STDOUT STDERR DUMP)")

IF (NOT ("${TMP_RETCODE1}" STREQUAL "0") OR
    NOT ("${TMP_RETCODE2}" STREQUAL "0"))
  MESSAGE("Maven Executable (NOTE: may need full path!): ${PSFLSL_MAVEN_COMMAND}")
  MESSAGE("Error Code: ${TMP_RETCODE1},${TMP_RETCODE2}")
  MESSAGE(FATAL_ERROR "Error Invoking Maven")
ELSE ()
  MESSAGE("Deploy Succeeded")
ENDIF ()
