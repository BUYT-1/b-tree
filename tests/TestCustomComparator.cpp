#include "gtest/gtest.h"
#include "b_tree.h"

struct SomeStruct {
    int n;
};

struct SomeComparator {
    bool operator()(SomeStruct a, SomeStruct b) const {
        return a.n < b.n;
    }
};

TEST(ComparatorSuite, ComparatorWorks) {
    b_tree::BTree<SomeStruct, 6, SomeComparator> a;
    a.insert({.n = 2});
    a.insert({.n = 4});
    a.insert({.n = 10});
    a.insert({.n = -31});
    a.insert({.n = 2000});
    a.insert({.n = -142});
    a.insert({.n = 0});
    a.insert({.n = 3});
    a.insert({.n = 3});
    ASSERT_TRUE(a.contains({.n = 0}));
    ASSERT_FALSE(a.contains({.n = 1}));
    ASSERT_TRUE(a.contains({.n = -142}));
    ASSERT_TRUE(a.contains({.n = 2000}));
    ASSERT_FALSE(a.contains({.n = 1999}));
    a.remove({.n = 0});
    ASSERT_FALSE(a.contains({.n = 0}));
}
