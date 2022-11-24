#include "gtest/gtest.h"
#include "b_tree.h"

class SameValuesSuite : public testing::Test {
protected:
    const size_t elements = 50;
    const std::string copium = "copium";
    b_tree::BTree<std::string, 2> tree{};

    SameValuesSuite() {
        for (size_t i = 0; i < elements; ++i) {
            tree.insert(copium);
        }
    }
};

TEST_F(SameValuesSuite, Find) {
    EXPECT_TRUE(tree.contains(copium));

    auto it = tree.find(copium);
    for (size_t i = 0; i < elements; ++i) {
        ASSERT_NE(it, tree.end());
        EXPECT_EQ(*it, copium);
        ++it;
    }
    EXPECT_EQ(it, tree.end());
}

TEST_F(SameValuesSuite, Iterate) {
    auto it = tree.begin();
    for (size_t i = 0; i < elements; ++i) {
        ASSERT_NE(it, tree.end());
        EXPECT_EQ(*it, copium);
        ++it;
    }
    EXPECT_EQ(it, tree.end());
}

TEST_F(SameValuesSuite, ReverseIterate) {
    auto it = tree.rbegin();
    for (size_t i = 0; i < elements; ++i) {
        ASSERT_NE(it, tree.rend());
        EXPECT_EQ(*it, copium);
        ++it;
    }
    EXPECT_EQ(it, tree.rend());
}

TEST_F(SameValuesSuite, Delete) {
    for (size_t i = 0; i < elements; ++i) {
        tree.remove(copium);
    }
    EXPECT_TRUE(tree.empty());
}
