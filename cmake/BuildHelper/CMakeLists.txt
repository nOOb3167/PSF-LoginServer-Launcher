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

LIST(APPEND CMAKE_MODULE_PATH "${PSFLSL_LOGINSERVER_SOURCE_DIR}/cmake/Modules/")

SET(PSFLSL_BUILDHELPER_BUILD_X32_DIR "${PSFLSL_LOGINSERVER_SOURCE_DIR}/cmake/Build_x32/" CACHE PATH "Build directory (x32)")
SET(PSFLSL_BUILDHELPER_BUILD_X64_DIR "${PSFLSL_LOGINSERVER_SOURCE_DIR}/cmake/Build_x64/" CACHE PATH "Build directory (x64)")

IF (NOT (EXISTS "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}"))
  MESSAGE("Creating Directory: "  "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}")
  FILE(MAKE_DIRECTORY "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}")
ENDIF ()
IF (NOT (EXISTS "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}"))
  MESSAGE("Creating Directory: "  "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}")
  FILE(MAKE_DIRECTORY "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}")
ENDIF ()

# removing just the CMakeCache file theoretically leaves
#   old build byproducts inside the build tree
# however anomalies resulting from the above are caused build script bugs
IF (EXISTS "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}/CMakeCache.txt")
  FILE(REMOVE "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}/CMakeCache.txt")
ENDIF ()
IF (EXISTS "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}/CMakeCache.txt")
  FILE(REMOVE "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}/CMakeCache.txt")
ENDIF ()


SET(PSFLSL_GENERATOR "${CMAKE_GENERATOR}")

IF (NOT (PSFLSL_GENERATOR MATCHES "^Visual Studio"))
  MESSAGE(FATAL_ERROR "Needs to be reworked for non-VS platforms")
ELSE ()
  # Legacy CMake feature: VS Generator names end in Win64 or ARM to select generator platform.
  STRING(REGEX REPLACE "^(.*) Win64" "\\1" PSFLSL_GENERATOR "${PSFLSL_GENERATOR}")
  STRING(REGEX REPLACE "^(.*) ARM"   "\\1" PSFLSL_GENERATOR "${PSFLSL_GENERATOR}")
ENDIF ()

IF (CMAKE_GENERATOR_PLATFORM)
  # Newer CMake versions: CMAKE_GENERATOR_PLATFORM is set to select generator platform.
  MESSAGE(SEND_ERROR "Platform mechanism not supported (CMAKE_GENERATOR_PLATFORM): "
    "Select specific generator (cmake -G option) instead.")
ENDIF ()

MESSAGE("Original Generator (CMAKE_GENERATOR): "  "${CMAKE_GENERATOR}")
MESSAGE("Computed Generator (PSFLSL_GENERATOR): " "${PSFLSL_GENERATOR}")

SET(PSFLSL_CONFIGURE_OPTS_COMMON
  -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

ADD_CUSTOM_TARGET(Configure_x32
  COMMAND
    "${CMAKE_COMMAND}"
      -G "${PSFLSL_GENERATOR}"
      # -A option omitted for x32
      "${PSFLSL_CONFIGURE_OPTS_COMMON}"
      "${PSFLSL_LOGINSERVER_SOURCE_DIR}"
  WORKING_DIRECTORY "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}"
)

ADD_CUSTOM_TARGET(Configure_x64
  COMMAND
    "${CMAKE_COMMAND}"
      -G "${PSFLSL_GENERATOR}"
      -A "x64"
      "${PSFLSL_CONFIGURE_OPTS_COMMON}"
      "${PSFLSL_LOGINSERVER_SOURCE_DIR}"
  WORKING_DIRECTORY "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}"
)

ADD_CUSTOM_TARGET(Build_x32
  ALL
  COMMAND
    "${CMAKE_COMMAND}"
      --build "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}"
      --clean-first
      --target INSTALL
  WORKING_DIRECTORY "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}"
)

ADD_CUSTOM_TARGET(Build_x64
  ALL
  COMMAND
    "${CMAKE_COMMAND}"
      --build "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}"
      --clean-first
      --target INSTALL
  WORKING_DIRECTORY "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}"
)

ADD_DEPENDENCIES(Build_x32 Configure_x32)
ADD_DEPENDENCIES(Build_x64 Configure_x64)
