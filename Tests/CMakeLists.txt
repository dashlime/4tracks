enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 COMPONENTS Test)

add_executable(4tracks_tests
    tst_audiotrack.cpp
)

target_link_libraries(4tracks_tests PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Test
    4tracks_lib
)
