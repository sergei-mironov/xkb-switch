CMAKE_MINIMUM_REQUIRED(VERSION 2.6)


PROJECT( XKB-SWITCH )

find_program(CTAGS ctags)

ADD_EXECUTABLE(xkb-switch XKbSwitch.cpp XKeyboard.cpp)

TARGET_LINK_LIBRARIES(xkb-switch X11)

add_custom_target(ctags
    COMMAND ${CTAGS} -R ${CMAKE_SOURCE_DIR}/*
)

#ADD_DEFINITIONS(-fpermissive)

