file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false *.cpp *.h *.ui)
file(GLOB_RECURSE RESOURCES LIST_DIRECTORIES false *.qrc)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (CMAKE_BUILD_TYPE MATCHES Coverage)
    set(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage")
endif ()

if (UNIX)
    set(LIBS
            Qt6::Core
            Qt6::Gui
            Qt6::Xml
            Qt6::Widgets
            juce_static_lib
            asound
            curl
            dl
            gcov)
elseif (WIN32)
    set(LIBS
            Qt6::Core
            Qt6::Gui
            Qt6::Xml
            Qt6::Widgets
            juce_static_lib
            gdi32
            user32
            kernel32
            comctl32
            uuid
            wsock32
            wininet
            version
            ole32
            ws2_32
            oleaut32
            imm32
            comdlg32
            shlwapi
            rpcrt4
            winmm
            gcov)
endif ()

# create static lib for unit tests
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
add_library(4tracks_lib STATIC ${SOURCES})
target_link_libraries(4tracks_lib PUBLIC ${LIBS})

add_executable(4tracks main.cpp ${RESOURCES})
target_link_libraries(4tracks PUBLIC ${LIBS} 4tracks_lib)

if (CMAKE_BUILD_TYPE MATCHES Coverage)
    target_link_options(4tracks PUBLIC --coverage)
endif ()