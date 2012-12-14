CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( XKB-SWITCH )

FIND_PROGRAM(CTAGS ctags)

ADD_EXECUTABLE(xkb-switch XKbSwitch.cpp XKeyboard.cpp)

TARGET_LINK_LIBRARIES(xkb-switch X11)

OPTION(BUILD_XKBSWITCH_LIB
    "Build a library compatible with vim's libcall interface" ON)
if(BUILD_XKBSWITCH_LIB)
    SET(xkblib xkbswitch)
    ADD_LIBRARY(${xkblib} SHARED XKbSwitchApi.cpp XKeyboard.cpp)
    TARGET_LINK_LIBRARIES(${xkblib} X11)
endif()

ADD_CUSTOM_TARGET(ctags
    COMMAND ${CTAGS} -R ${CMAKE_SOURCE_DIR}/*
)

INSTALL(TARGETS xkb-switch ${xkblib}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib OPTIONAL
)

