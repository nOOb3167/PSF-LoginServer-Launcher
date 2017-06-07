CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

PROJECT (PSF-LoginServer-Launcher)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

FIND_PACKAGE(Shlwapi REQUIRED)

INCLUDE(CheckTypeSize)
CHECK_TYPE_SIZE("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)
IF (SIZEOF_VOID_P EQUAL 8)
  SET(PSFLSL_ARCH_BITNESS 64)
ELSE ()
  SET(PSFLSL_ARCH_BITNESS 32)
ENDIF ()

FIND_PATH(PSFLSL_JNI_HEADER_INCLUDE_DIR
  NAMES jni.h
  PATHS data/jni_external/
  DOC "Directory where jni.h is located (Will be put on include path). \
       jni.h does come included with the Java JDK. \
       However it is also bundled with this project as fallback."
)
FIND_PATH(PSFLSL_JNI_MD_HEADER_INCLUDE_DIR
  NAMES jni_md.h PATHS data/jni_external/win32/ DOC "See doc for jni.h")
IF (NOT (PSFLSL_JNI_HEADER_INCLUDE_DIR AND PSFLSL_JNI_MD_HEADER_INCLUDE_DIR))
  MESSAGE(SEND_ERROR "jni.h not found")
ENDIF ()

FILE(READ "${CMAKE_CURRENT_SOURCE_DIR}/data/PSF-LoginServer-Launcher-Config.txt" EXTERNAL_PSFLSL_CONFIG_BUILTIN_HEXSTRING HEX)

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
  PSF-LoginServer-Launcher-Config.txt
)

ADD_CUSTOM_TARGET(PSF-Target-ConfigFile
  DEPENDS PSF-LoginServer-Launcher-Config.txt
)

ADD_CUSTOM_COMMAND(
  OUTPUT PSF-LoginServer-Launcher-Config.txt
  DEPENDS PSF-LoginServer-Launcher-ConfigHeaderGen
          "${CMAKE_CURRENT_SOURCE_DIR}/data/PSF-LoginServer-Launcher-Config.txt"
  COMMAND PSF-LoginServer-Launcher-ConfigHeaderGen
    "${CMAKE_CURRENT_SOURCE_DIR}/data/PSF-LoginServer-Launcher-Config.txt"
    "${CMAKE_CURRENT_BINARY_DIR}/PSF-LoginServer-Launcher-Config.txt"
  COMMENT "Generating Launcher Config"
)

ADD_DEPENDENCIES(PSF-LoginServer-Launcher PSF-Target-ConfigFile)

TARGET_COMPILE_DEFINITIONS(PSF-LoginServer-Launcher
  PRIVATE -DEXTERNAL_PSFLSL_CONFIG_BUILTIN_HEXSTRING="${EXTERNAL_PSFLSL_CONFIG_BUILTIN_HEXSTRING}"
  PRIVATE -DEXTERNAL_PSFLSL_ARCH_BITNESS=${PSFLSL_ARCH_BITNESS}
)
  
TARGET_INCLUDE_DIRECTORIES(PSF-LoginServer-Launcher
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
  PRIVATE "${PSFLSL_JNI_HEADER_INCLUDE_DIR}"
  PRIVATE "${PSFLSL_JNI_MD_HEADER_INCLUDE_DIR}"
)

TARGET_LINK_LIBRARIES(PSF-LoginServer-Launcher ${SHLWAPI_LIBRARY})

SET_TARGET_PROPERTIES(PSF-LoginServer-Launcher
  PROPERTIES
  OUTPUT_NAME "PSF-LoginServer-Launcher_x${PSFLSL_ARCH_BITNESS}"
)

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
