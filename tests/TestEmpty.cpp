#include <limits>

#include "gtest/gtest.h"
#include "b_tree.h"

TEST(EmptySuite, FindNothingInt) {
    b_tree::BTree<int, 100> tree;
    ASSERT_FALSE(tree.contains(0));
    ASSERT_FALSE(tree.contains(-1));
    ASSERT_FALSE(tree.contains(1));
    ASSERT_FALSE(tree.contains(100));
    ASSERT_FALSE(tree.contains(std::numeric_limits<int>::max()));
    ASSERT_FALSE(tree.contains(std::numeric_limits<int>::min()));
}

TEST(EmptySuite, FindNothingSizeT) {
    b_tree::BTree<size_t, 3> tree;
    ASSERT_FALSE(tree.contains(0));
    ASSERT_FALSE(tree.contains(-1));
    ASSERT_FALSE(tree.contains(1));
    ASSERT_FALSE(tree.contains(100));
    ASSERT_FALSE(tree.contains(std::numeric_limits<size_t>::max()));
    ASSERT_FALSE(tree.contains(std::numeric_limits<size_t>::min()));
}

TEST(EmptySuite, FindNothingString) {
    b_tree::BTree<std::string , 3> tree;
    ASSERT_FALSE(tree.contains(""));
    ASSERT_FALSE(tree.contains("abc"));
    ASSERT_FALSE(tree.contains("words three words"));
    ASSERT_FALSE(tree.contains(" "));
    ASSERT_FALSE(tree.contains("btree"));
}

TEST(EmptySuite, RemoveNothingInt) {
    b_tree::BTree<int, 100> tree;
    tree.remove(0);
    tree.remove(-1);
    tree.remove(1);
    tree.remove(100);
    tree.remove(std::numeric_limits<int>::max());
    tree.remove(std::numeric_limits<int>::min());
}

TEST(EmptySuite, RemoveNothingString) {
    b_tree::BTree<std::string , 3> tree;
    tree.remove("");
    tree.remove("abc");
    tree.remove("words three words");
    tree.remove(" ");
    tree.remove("btree");
}
