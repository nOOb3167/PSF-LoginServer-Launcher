## - Massage PSFLSL_GENERATOR
## - Define targets

IF (NOT DEFINED CMAKE_INSTALL_PREFIX             OR
    NOT DEFINED PSFLSL_LOGINSERVER_SOURCE_DIR    OR
    NOT DEFINED PSFLSL_BUILDHELPER_BUILD_X32_DIR OR
    NOT DEFINED PSFLSL_BUILDHELPER_BUILD_X64_DIR OR
    NOT DEFINED PSFLSL_GENERATOR)
  MESSAGE(FATAL_ERROR "May have been INCLUDEd without setting the necessary variables")
ENDIF ()

# The chosen generator (PSFLSL_GENERATOR) must be one of the Visual Studio generators.
# PSFLSL_GENERATOR will be stripped of any platform name suffix.
#   https://cmake.org/cmake/help/v3.9/generator/Visual%20Studio%2012%202013.html
#   "... one may specify a target platform name optionally at the end of this generator name ..."

SET(PSFLSL_GENERATOR_COMPUTED "${PSFLSL_GENERATOR}")
STRING(REGEX REPLACE "^(.*) Win64" "\\1" PSFLSL_GENERATOR_COMPUTED "${PSFLSL_GENERATOR_COMPUTED}")
STRING(REGEX REPLACE "^(.*) ARM"   "\\1" PSFLSL_GENERATOR_COMPUTED "${PSFLSL_GENERATOR_COMPUTED}")

MESSAGE("Original Generator (PSFLSL_GENERATOR): "          "${CMAKE_GENERATOR}")
MESSAGE("Computed Generator (PSFLSL_GENERATOR_COMPUTED): " "${PSFLSL_GENERATOR_COMPUTED}")

# Add targets that will configure and build the 'parent' PSFLSL project.
# (Parent in the context of this file being INCLUDEd as part of BuildHelper)
# For the configure step, targets invoke CMake, passing in the correct install prefix,
# and possibly with the -A option (Select 32 or 64 bit build for Visual Studio generators)
# For the build step, the --build option (build-tool-mode) is passed.
# https://cmake.org/cmake/help/v3.8/manual/cmake.1.html#build-tool-mode
#   The documentation is not superb but basically it invokes make (for makefiles)
#   or msbuild (for Visual Studio solutions - relevant here)

# Both targets (32 and 64 bit) are configured with the same install prefix.
# But the build directories for the two separare builds must be separate.
#   Further, while CMake will attempt creating the install directory structure
#   when performing an INSTALL, this seems to not be done for the build directory
#   (at least not when invoking CMake by passing WORKING_DIRECTORY into
#   an ADD_CUSTOM_TARGET command), resulting in a failed build.
# Therefore ensure existence of the build directories here.

IF (NOT (EXISTS "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}"))
  MESSAGE("Creating Directory: "  "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}")
  FILE(MAKE_DIRECTORY "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}")
ENDIF ()
IF (NOT (EXISTS "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}"))
  MESSAGE("Creating Directory: "  "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}")
  FILE(MAKE_DIRECTORY "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}")
ENDIF ()

ADD_CUSTOM_TARGET(Configure_x32
  COMMAND
    "${CMAKE_COMMAND}"
      -G "${PSFLSL_GENERATOR_COMPUTED}"
      # -A option omitted for x32
      "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
      "${PSFLSL_LOGINSERVER_SOURCE_DIR}"
  WORKING_DIRECTORY
    "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}"
)

ADD_CUSTOM_TARGET(Configure_x64
  COMMAND
    "${CMAKE_COMMAND}"
      -G "${PSFLSL_GENERATOR_COMPUTED}"
      -A "x64"
      "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
      "${PSFLSL_LOGINSERVER_SOURCE_DIR}"
  WORKING_DIRECTORY
    "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}"
)

ADD_CUSTOM_TARGET(Build_x32
  ALL
  COMMAND
    "${CMAKE_COMMAND}"
      --build "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}"
      --clean-first
      --target INSTALL
  WORKING_DIRECTORY
    "${PSFLSL_BUILDHELPER_BUILD_X32_DIR}"
)

ADD_CUSTOM_TARGET(Build_x64
  ALL
  COMMAND
    "${CMAKE_COMMAND}"
      --build "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}"
      --clean-first
      --target INSTALL
  WORKING_DIRECTORY
    "${PSFLSL_BUILDHELPER_BUILD_X64_DIR}"
)

ADD_DEPENDENCIES(Build_x32 Configure_x32)
ADD_DEPENDENCIES(Build_x64 Configure_x64)
