SET(PSFLSL_VERSION "0.0.1")


FUNCTION (PSFLSL_VERSION_PROCESS PSFLSL_MAVEN_ARTIFACT_VERSION OUTVARNAME)
  # Process PSFLSL_MAVEN_ARTIFACT_VERSION. (Counts as defaulted if empty string)
  # When defaulted, the version used will be the result of suffixing PSFLSL_VERSION with '-SNAPSHOT'.
  # When not defaulted, the version used will be the value of this variable directly,
  #   EXCEPT any '@VER@' becoming replaced by PSFLSL_VERSION.
  
  IF (NOT "${PSFLSL_MAVEN_ARTIFACT_VERSION}")
    SET("${OUTVARNAME}" "${PSFLSL_VERSION}-SNAPSHOT")
  ELSE ()
    STRING(REPLACE "@VER@"
      "${PSFLSL_VERSION}" "${OUTVARNAME}"
      "${PSFLSL_MAVEN_ARTIFACT_VERSION}"
    )
  ENDIF ()
  
  SET("${OUTVARNAME}" "${${OUTVARNAME}}" PARENT_SCOPE)
ENDFUNCTION ()
