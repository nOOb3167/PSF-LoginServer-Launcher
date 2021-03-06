# This CMakeLists.txt builds the Launcher executable.
# To build the 'full' distribution as designed, see BuildHelper (cmake/BuildHelper/CMakeLists.txt).
#
# The following variables can be customized:
#   CMAKE_INSTALL_PREFIX - Install location.
#   PSFLSL_DISABLE_CONFIG_GENERATION - ON/OFF - Use a pre-generated (empty) config, instead of generating,
#                                               from the config templates within data/.
#   PSFLSL_ARCH_BITNESS - 32 or 64 - Normally autodetected. Whether a 32-bit or 64-bit executable is being built.
#                                    Influences selection of the config template. Bitness is also built into the
#                                    executable filename.
#   PSFLSL_JNI_HEADER_INCLUDE_DIR - Normally autodetect and use the JNI headers bundled within data/jni_external/.
#                                   Launcher operates by dynamically loading the JVM library (jvm.dll), from an
#                                   existing Java installation already present on the user computer.
#                                   Let us say we compile with JNI headers of Java version X.
#                                   Obviously the JVM library on another computer running the Launcher may be different.
#                                   Therefore the JNI headers MUST have already been designed for binary compatibility
#                                   accross different Java versions in mind. Thus we can bundle some arbitrary version.
#   PSFLSL_JNI_MD_HEADER_INCLUDE_DIR - See comment for PSFLSL_JNI_HEADER_INCLUDE_DIR.
#
# Example directory structure at CMAKE_INSTALL_PREFIX, after the install target has been built:
#   ${CMAKE_INSTALL_PREFIX}/bin/PSF-LoginServer-Launcher_x32.exe  - Launcher executable (32-bit)
#   ${CMAKE_INSTALL_PREFIX}/bin/PSF-LoginServer-Launcher_x32.pdb  - (Visual Studio debug file)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

PROJECT (PSF-LoginServer-Launcher)

OPTION(PSFLSL_DISABLE_CONFIG_GENERATION "Use an empty default config instead of generating one" OFF)

# use custom find module for Shlwapi

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

FIND_PACKAGE(Shlwapi REQUIRED)

# infer bitness of compiler selected by cmake from native pointer size

INCLUDE(CheckTypeSize)
CHECK_TYPE_SIZE("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)
IF (SIZEOF_VOID_P EQUAL 8)
  SET(PSFLSL_ARCH_BITNESS 64 CACHE STRING "Bitness (32 or 64)")
ELSE ()
  SET(PSFLSL_ARCH_BITNESS 32 CACHE STRING "Bitness (32 or 64)")
ENDIF ()

# find the JNI headers - or use bundled

FIND_PATH(PSFLSL_JNI_HEADER_INCLUDE_DIR
  NAMES jni.h
  PATHS "${CMAKE_SOURCE_DIR}/data/jni_external/"
  CMAKE_FIND_ROOT_PATH_BOTH
  DOC "Directory where jni.h is located (Will be put on include path). \
       jni.h does come included with the Java JDK. \
       However it is also bundled with this project as fallback."
)
FIND_PATH(PSFLSL_JNI_MD_HEADER_INCLUDE_DIR
  NAMES jni_md.h
  PATHS "${CMAKE_SOURCE_DIR}/data/jni_external/win32/"
  CMAKE_FIND_ROOT_PATH_BOTH
  DOC "See doc for jni.h")
IF (NOT (PSFLSL_JNI_HEADER_INCLUDE_DIR AND PSFLSL_JNI_MD_HEADER_INCLUDE_DIR))
  MESSAGE(SEND_ERROR "jni.h not found")
ENDIF ()

# create the Launcher and Config Header Generator targets - with sources

SET(PSFLSL_HEADERS
  include/psflsl/config.h
  include/psflsl/filesys.h
  include/psflsl/misc.h
  include/psflsl/runner.h
  include/psflsl/registry.h
)

SET(PSFLSL_SOURCES
  src/config.cpp
  src/filesys.cpp
  src/misc.cpp
  src/runner.cpp
  src/registry.cpp
  src/main.cpp
)

ADD_EXECUTABLE(PSF-LoginServer-Launcher-ConfigHeaderGen
  src/gen/config_header_gen.cpp
)

ADD_EXECUTABLE(PSF-LoginServer-Launcher
  ${PSFLSL_HEADERS}
  ${PSFLSL_SOURCES}
  PSF-LoginServer-Launcher-Config.h
)

# Config Header Generator stuff

IF (PSFLSL_DISABLE_CONFIG_GENERATION)
  SET(PSFLSL_CONFIG_NAME "PSF-LoginServer-Launcher-Config-PreGenerated-Default.h")

  ADD_CUSTOM_COMMAND(
    OUTPUT PSF-LoginServer-Launcher-Config.h
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/${PSFLSL_CONFIG_NAME}"
    COMMAND "${CMAKE_COMMAND}"
      -E copy_if_different
      "${CMAKE_CURRENT_SOURCE_DIR}/data/${PSFLSL_CONFIG_NAME}"
      "${CMAKE_CURRENT_BINARY_DIR}/PSF-LoginServer-Launcher-Config.h"
    COMMENT "Generating Launcher Config (Empty default)"
  )  
ELSE ()
  SET(PSFLSL_CONFIG_NAME "PSF-LoginServer-Launcher-Config_x${PSFLSL_ARCH_BITNESS}.txt")

  ADD_CUSTOM_COMMAND(
    OUTPUT PSF-LoginServer-Launcher-Config.h
    DEPENDS PSF-LoginServer-Launcher-ConfigHeaderGen
            "${CMAKE_CURRENT_SOURCE_DIR}/data/${PSFLSL_CONFIG_NAME}"
    COMMAND PSF-LoginServer-Launcher-ConfigHeaderGen
      "${CMAKE_CURRENT_SOURCE_DIR}/data/${PSFLSL_CONFIG_NAME}"
      "${CMAKE_CURRENT_BINARY_DIR}/PSF-LoginServer-Launcher-Config.h"
    COMMENT "Generating Launcher Config"
  )  
ENDIF ()

# target configuration stuff (gcc options such as -D, -I, -L, -o)

TARGET_COMPILE_DEFINITIONS(PSF-LoginServer-Launcher
  PRIVATE -DEXTERNAL_PSFLSL_ARCH_BITNESS=${PSFLSL_ARCH_BITNESS}
)
  
TARGET_INCLUDE_DIRECTORIES(PSF-LoginServer-Launcher
  PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"  # for Config Header Generator
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
  PRIVATE "${PSFLSL_JNI_HEADER_INCLUDE_DIR}"
  PRIVATE "${PSFLSL_JNI_MD_HEADER_INCLUDE_DIR}"
)

TARGET_LINK_LIBRARIES(PSF-LoginServer-Launcher ${SHLWAPI_LIBRARY})

SET_TARGET_PROPERTIES(PSF-LoginServer-Launcher
  PROPERTIES
  OUTPUT_NAME "PSF-LoginServer-Launcher_x${PSFLSL_ARCH_BITNESS}"
)

# target install definition stuff

INSTALL(TARGETS PSF-LoginServer-Launcher
  LIBRARY DESTINATION "lib"
  RUNTIME DESTINATION "bin"
  ARCHIVE DESTINATION "lib"
)

# Visual Studio CMake generator specific stuff

IF (MSVC)
    ## categorize files in project tab
    SOURCE_GROUP("Header Files" FILES ${PSFLSL_HEADERS})
    SOURCE_GROUP("Source Files" FILES ${PSFLSL_SOURCES})
    ## copy pdb debug files alongside executable when installing
    INSTALL(FILES
      "$<TARGET_PDB_FILE:PSF-LoginServer-Launcher>"
      DESTINATION "bin"
    )
ENDIF ()
