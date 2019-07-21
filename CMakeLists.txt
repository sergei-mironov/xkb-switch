CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( XKB-SWITCH )
SET(MAJOR_VERSION 1)
SET(MINOR_VERSION 5)
SET(RELEASE_VERSION 3)
SET(XKBSWITCH_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${RELEASE_VERSION})
ADD_DEFINITIONS(-DXKBSWITCH_VERSION="${XKBSWITCH_VERSION}")

# Check presence of development libraries required for build
FIND_PACKAGE(X11 REQUIRED)
if(NOT X11_FOUND)
    message(FATAL_ERROR "Not found development files of 'libx11' required for build. (Install libx11-dev or libx11-devel package.) CMake will exit.")
elseif(NOT X11_Xkbfile_FOUND)
    message(FATAL_ERROR "Not found development files of 'libxkbfile' required for build. (Install libxkbfile-dev or libxkbfile-devel package.) CMake will exit.")
endif()

# Compile and link program
OPTION(BUILD_XKBSWITCH_LIB
    "Build a library compatible with vim's libcall interface" ON)
if(BUILD_XKBSWITCH_LIB)
    SET(xkblib xkbswitch)
    ADD_LIBRARY(${xkblib} SHARED src/XKbSwitchApi.cpp src/XKeyboard.cpp)
    SET_TARGET_PROPERTIES(${xkblib} PROPERTIES VERSION ${XKBSWITCH_VERSION} SOVERSION ${MAJOR_VERSION})
    TARGET_LINK_LIBRARIES(${xkblib} X11 xkbfile)
    ADD_EXECUTABLE(xkb-switch src/XKbSwitch.cpp)
    TARGET_LINK_LIBRARIES(xkb-switch ${xkblib})
else()
    ADD_EXECUTABLE(xkb-switch src/XKbSwitch.cpp src/XKeyboard.cpp)
    TARGET_LINK_LIBRARIES(xkb-switch X11 xkbfile)
endif()

# Install program
INSTALL(TARGETS xkb-switch ${xkblib}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib OPTIONAL
)

# Function to compress and install man page
# Gets file name and type number
function(install_man man_filename man_type)
    # check what compression tool is available
    SET(MAN_COMPRESSION xz)
    FIND_PROGRAM(COMPRESS_EXECUTABLE NAMES xz)
    if(NOT COMPRESS_EXECUTABLE)
        FIND_PROGRAM(COMPRESS_EXECUTABLE NAMES gzip)
        if(COMPRESS_EXECUTABLE)
            SET(MAN_COMPRESSION gz)
        else()
            SET(MAN_COMPRESSION NO)
        endif()
    endif()
    # set input an output file names
    SET(raw_man man/${man_filename}.${man_type})
    SET(compressed_man ${CMAKE_BINARY_DIR}/${man_filename}.${man_type}.${MAN_COMPRESSION})
    # compress if there is the compression tool
    if(MAN_COMPRESSION)
        ADD_CUSTOM_COMMAND(OUTPUT ${compressed_man}
            COMMAND cat ${raw_man} | ${COMPRESS_EXECUTABLE} > ${compressed_man}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            DEPENDS ${raw_man}
            COMMENT "Compressing man file ${raw_man} to ${compressed_man}"
        )
    # elsewise just copy
    else()
        MESSAGE(WARNING "There is no compression tool for man pages (xz or gzip). Not compressed copy of man file will be used.")
        SET(compressed_man ${CMAKE_BINARY_DIR}/${man_filename}.${man_type})
        ADD_CUSTOM_COMMAND(OUTPUT ${compressed_man}
            COMMAND cp ${raw_man} ${CMAKE_BINARY_DIR}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            DEPENDS ${raw_man}
            COMMENT "Copying man file from ${raw_man}."
        )
    endif()
    # add actions
    ADD_CUSTOM_TARGET(man_${man_filename}_${man_type} ALL DEPENDS ${compressed_man})
    INSTALL(FILES ${compressed_man}
        DESTINATION share/man/man${man_type}
    )
endfunction()

# Compress and install man page
install_man(xkb-switch 1)
