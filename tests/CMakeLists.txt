project(B_tree_tests)

add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR})

add_executable(b_tree_test TestEmpty.cpp TestInsert.cpp TestDelete.cpp TestIterate.cpp TestFind.cpp
        TestCustomComparator.cpp TestCopy.cpp TestMove.cpp TestSameValues.cpp TestHuge.cpp)

target_link_libraries(b_tree_test gtest gtest_main)
