file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false *.cpp *.h *.ui *.qrc)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage")

set(LIBS
        Qt6::Core
        Qt6::Gui
        Qt6::Xml
        Qt6::Widgets
        juce_static_lib
        asound
        curl
        dl
	gcov
        )

# create static lib for unit tests
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
add_library(4tracks_lib STATIC ${SOURCES})
target_link_libraries(4tracks_lib PUBLIC ${LIBS})

add_executable(4tracks main.cpp)
target_link_libraries(4tracks PUBLIC ${LIBS} 4tracks_lib)
target_link_options(4tracks PUBLIC --coverage)
