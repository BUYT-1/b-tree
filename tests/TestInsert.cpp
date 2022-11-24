#include "gtest/gtest.h"
#include "b_tree.h"

class InsertSuite : public testing::Test {
protected:
    b_tree::BTree<size_t, 10> tree;

    void expect_find(size_t i, size_t to) {
        for (; i < to; ++i) {
            EXPECT_TRUE(tree.contains(i));
        }
    }

    void expect_find_reverse(size_t i, size_t to) {
        for (; i > to; --i) {
            EXPECT_TRUE(tree.contains(i));
        }
        EXPECT_TRUE(tree.contains(to));
    }

    void expect_no_find(size_t i, size_t to) {
        for (; i < to; ++i) {
            EXPECT_FALSE(tree.contains(i));
        }
    }

    void expect_no_find_reverse(size_t i, size_t to) {
        for (; i > to; --i) {
            EXPECT_FALSE(tree.contains(i));
        }
        EXPECT_FALSE(tree.contains(to));
    }
};

TEST_F(InsertSuite, SortedInsertionNoSplitSizeT) {
    for (size_t i = 0; i < 10; ++i) {
        tree.insert(i);
    }

    expect_find(0, 10);
    expect_no_find(10, 100);
}

TEST_F(InsertSuite, ReverseSortedInsertionNoSplitSizeT) {
    for (size_t i = 10; i > 0; --i) {
        tree.insert(i - 1);
    }

    expect_find(0, 10);
    expect_no_find(10, 100);
}

TEST_F(InsertSuite, SortedInsertionSplitSizeT) {
    for (size_t i = 0; i < 1923; ++i) {
        tree.insert(i);
    }

    expect_find_reverse(1828, 0);
    expect_no_find(1923, 2050);
    expect_no_find_reverse(3456, 1923);
}

TEST_F(InsertSuite, ReverseSortedInsertionSplitSizeT) {
    for (size_t i = 1922; i > 0; --i) {
        tree.insert(i);
    }
    tree.insert(0);

    expect_find_reverse(1828, 0);
    expect_no_find(1923, 2050);
}

TEST_F(InsertSuite, ConvergingInsertionNoSplitSizeT) {
    for (size_t i = 0; i < 5; ++i) {
        tree.insert(i);
        tree.insert(9 - i);
    }

    expect_find(0, 10);
    expect_no_find(10, 100);
}
