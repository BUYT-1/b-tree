#include <ranges>

#include "gtest/gtest.h"
#include "b_tree.h"

TEST(HugeSuite, SortedInsert) {
    /*b_tree::BTree<int, 50> tree;
    for (int i = -4000000; i < 4000000; ++i) {
        tree.insert(i);
    }*/
    /*for (int i = 9999; i >= -10000; --i) {
        tree.remove(i);
    }*/
}
/*
TEST(HugeSuite, Iteration) {
    long long result = 0;
    b_tree::BTree<long long, 63> tree;
    for (long long i = -5000; i < 5000; ++i) {
        tree.insert(i);
    }
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        result += *it;
    }
}
*/