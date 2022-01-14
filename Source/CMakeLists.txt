file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false *.cpp *.h *.ui *.qrc)

add_executable(4tracks ${SOURCES})

set(LIBS
        Qt6::Core
        Qt6::Gui
        Qt6::Xml
        Qt6::Widgets
        juce_static_lib
        asound
        curl
        dl
        )

target_link_libraries(4tracks PUBLIC ${LIBS})

# create static lib for unit tests

add_library(4tracks_lib STATIC ${SOURCES})
target_link_libraries(4tracks_lib PUBLIC ${LIBS})
