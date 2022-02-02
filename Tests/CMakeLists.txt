file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(4tracks_test ${TEST_SOURCES})
target_link_libraries(4tracks_test 4tracks_lib gtest gtest_main)