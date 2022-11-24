#include <algorithm>
#include <ranges>
#include <unordered_set>

#include "gtest/gtest.h"
#include "b_tree.h"

class RemoveSuite : public testing::Test {
protected:
    b_tree::BTree<long long, 17> ll_tree{};
    b_tree::BTree<size_t, 17> st_tree{};
    b_tree::BTree<short, 2> small_tree{};

    RemoveSuite() {
        for (auto i = -546l; i < 2124l; ++i) {
            long long number = i * i + i * 333 + i / 32 + (i >> 4) - (i * 318) + (i & 0x5F3759DF);
            ll_tree.insert(number);
            numbers.emplace_back(number);
        }
        sorted_numbers = numbers;
        std::sort(sorted_numbers.begin(), sorted_numbers.end());
        nums_ms = {sorted_numbers.begin(), sorted_numbers.end()};
    }

    std::vector<long long> numbers{};
    std::vector<long long> sorted_numbers{};
    std::unordered_multiset<long long> nums_ms{};
};

TEST_F(RemoveSuite, RemoveEverything) {
    for (auto num : sorted_numbers) {
        ll_tree.remove(num);
    }
    for (auto num : sorted_numbers) {
        EXPECT_FALSE(ll_tree.contains(num));
    }
}

TEST_F(RemoveSuite, RemoveSomeNumbersSortedOrder) {
    for (size_t i = 0; i < sorted_numbers.size() / 5; ++i) {
        ll_tree.remove(sorted_numbers[i]);
    }
    std::unordered_set<long long> in_both_parts{};
    auto ind = static_cast<long long>(sorted_numbers.size()) / 5;

    std::set_intersection(sorted_numbers.begin(), sorted_numbers.begin() + ind, sorted_numbers.begin() + ind,
                          sorted_numbers.end(), std::inserter(in_both_parts, in_both_parts.end()));
    for (size_t i = 0; i < sorted_numbers.size() / 5; ++i) {
        auto const num = sorted_numbers[i];
        if (in_both_parts.find(num) == in_both_parts.end()) {
            EXPECT_FALSE(ll_tree.contains(num));
        }
        else {
            EXPECT_TRUE(ll_tree.contains(num));
        }
    }
    for (size_t i = sorted_numbers.size() / 5; i < sorted_numbers.size(); ++i) {
        EXPECT_TRUE(ll_tree.contains(sorted_numbers[i]));
    }
}

TEST_F(RemoveSuite, RemoveSomeNumbersUnsortedOrder) {
    std::unordered_set<long long> removed{};
    for (size_t i = 0; i < 100; ++i) {
        auto value = sorted_numbers[i * 743 % sorted_numbers.size()];
        nums_ms.erase(nums_ms.find(value));
        ll_tree.remove(value);
        removed.insert(value);
    }

    for (auto num : removed) {
        if (nums_ms.find(num) == nums_ms.end()) {
            EXPECT_FALSE(ll_tree.contains(num)) << num;
        }
        else {
            EXPECT_TRUE(ll_tree.contains(num)) << num;
        }
    }
    for (auto num : nums_ms) {
        EXPECT_TRUE(ll_tree.contains(num));
    }
}

TEST_F(RemoveSuite, RemoveJiggle) {
    size_t const val = 8888888888888888;
    for (size_t i = 0; i < 10; ++i) {
        st_tree.insert(val);
        EXPECT_TRUE(st_tree.contains(val));
        st_tree.remove(val);
        EXPECT_FALSE(st_tree.contains(val));
        EXPECT_TRUE(st_tree.empty());
    }
}

TEST_F(RemoveSuite, ReverseSortedRemove) {
    for (long long sorted_number : std::ranges::reverse_view(sorted_numbers)) {
        ll_tree.remove(sorted_number);
    }
    EXPECT_TRUE(ll_tree.empty());
}

TEST_F(RemoveSuite, BeginEndRemove) {
    auto sz = sorted_numbers.size();
    for (size_t i = 0; i < sz / 2; ++i) {
        ll_tree.remove(sorted_numbers[sz - i - 1]);
        ll_tree.remove(sorted_numbers[i]);
    }
    EXPECT_TRUE(ll_tree.empty());
}

TEST_F(RemoveSuite, TakeFromRight) {
    // The situation in which you have to take a value from
    // the right node is not common. This tests that case.
    b_tree::BTree<int, 2> tree;
    for (int i = 0; i < 16; ++i) {
        tree.insert(i);
    }
    for (int i = 7; i < 16; ++i) {
        tree.remove(i);
    }
    for (int i = 0; i < 7; ++i) {
        EXPECT_TRUE(tree.contains(i));
    }
    for (int i = 7; i < 16; ++i) {
        EXPECT_FALSE(tree.contains(i));
    }
}

TEST_F(RemoveSuite, RootOnly) {
    for (short i = 0; i < 3; ++i) {
        small_tree.insert(i);
    }
    for (short i = 0; i < 3; ++i) {
        small_tree.remove(i);
    }
    EXPECT_TRUE(small_tree.empty());
}

TEST_F(RemoveSuite, OneMoreThanSingleNode) {
    for (short i = 0; i < 4; ++i) {
        small_tree.insert(i);
    }
    for (short i = 0; i < 4; ++i) {
        small_tree.remove(i);
    }
    EXPECT_TRUE(small_tree.empty());
}

TEST_F(RemoveSuite, DeepMiddleRemove) {
    // Moving a key down more than once during deletion is uncommon.
    // This tests that case.
    for (short i = 0; i < 16; ++i) {
        small_tree.insert(i);
    }
    small_tree.remove(3);
    for (short i = 0; i < 16; ++i) {
        small_tree.remove(i);
    }
    EXPECT_TRUE(small_tree.empty());
}
