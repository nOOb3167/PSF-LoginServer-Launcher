CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

PROJECT (PSF-LoginServer-Launcher)

SET(PSFLSL_HEADERS
)

SET(PSFLSL_SOURCES
  src/main.cpp
)

ADD_EXECUTABLE(PSF-LoginServer-Launcher
  ${PSFLSL_HEADERS}
  ${PSFLSL_SOURCES}
)

TARGET_INCLUDE_DIRECTORIES(PSF-LoginServer-Launcher
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

INSTALL(TARGETS PSF-LoginServer-Launcher
  LIBRARY DESTINATION "lib"
  RUNTIME DESTINATION "bin"
  ARCHIVE DESTINATION "lib"
)

INSTALL(FILES "data/PSF-LoginServer-Launcher-Config.txt"
    DESTINATION "bin"
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
