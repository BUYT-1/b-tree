#include "gtest/gtest.h"
#include "b_tree.h"

TEST(MoveSuite, MoveAssignmentTest) {
    using TreeType = b_tree::BTree<int, 4>;
    TreeType tree;
    for (int i = 73; i > -51; --i) {
        tree.insert(i);
    }
    TreeType moved;
    ASSERT_TRUE(moved.empty());
    moved = std::move(tree);
    for (int i = 73; i > -51; --i) {
        EXPECT_TRUE(moved.contains(i));
    }
}

TEST(MoveSuite, MoveConstructionTest) {
    using TreeType = b_tree::BTree<signed char, 4>;
    TreeType tree;
    for (signed char i = 73; i > -51; --i) {
        tree.insert(i);
    }
    TreeType moved{std::move(tree)};
    for (signed char i = 73; i > -51; --i) {
        EXPECT_TRUE(moved.contains(i));
    }
}
