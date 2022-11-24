#include <algorithm>
#include <vector>
#include <unordered_set>

#include "gtest/gtest.h"
#include "b_tree.h"

class IterateSuite : public testing::Test {
protected:
    b_tree::BTree<double, 17> double_tree;
    b_tree::BTree<char, 9> char_tree{};

    IterateSuite() {
        for (auto i = -546.0; i < 2124l; i += 1.5) {
            double number = i + static_cast<float>(i / 6.1) + static_cast<int>(i * 3.1415926535897932384626433) % 3;
            long s = *(long *) &number;
            s = 0x5f3759df - (s >> 1);
            number = *(double *) &s;
            double_tree.insert(number);
            numbers.emplace_back(number);
        }
        sorted_numbers = numbers;
        std::sort(sorted_numbers.begin(), sorted_numbers.end());
        nums_ms = {sorted_numbers.begin(), sorted_numbers.end()};
        for (char let = 'a';  let < 'a' + 26; ++let) {
            char_tree.insert(let);
            sorted_chars.emplace_back(let);
        }
    }

    std::vector<double> numbers{};
    std::vector<double> sorted_numbers{};
    std::unordered_multiset<double> nums_ms{};
    std::vector<char> sorted_chars;
};

TEST_F(IterateSuite, IterationIsSorted) {
    auto snit = sorted_numbers.begin();
    auto btit = double_tree.begin();
    while (snit != sorted_numbers.end()) {
        ASSERT_NE(btit, double_tree.end());
        EXPECT_EQ(*btit, *snit);
        ++snit;
        ++btit;
    }
}

TEST_F(IterateSuite, ReverseIterationIsReverseSorted) {
    auto snit = sorted_numbers.rbegin();
    auto btit = double_tree.rbegin();
    while (snit != sorted_numbers.rend()) {
        ASSERT_NE(btit, double_tree.rend());
        EXPECT_EQ(*btit, *snit);
        ++snit;
        ++btit;
    }
}

TEST_F(IterateSuite, IncDecTest) {
    auto it1 = char_tree.begin();
    auto it2 = it1++;
    EXPECT_EQ(it2, char_tree.begin());
    it2 = it2;
    EXPECT_EQ(it2, char_tree.begin());
    EXPECT_EQ(it1, ++char_tree.begin());
    auto it3 = ++it2;
    EXPECT_EQ(it3, ++char_tree.begin());
    EXPECT_EQ(--it2, char_tree.begin());
    --it1 = it2;
    EXPECT_EQ(it2, it1);
    EXPECT_EQ(--it3, it1);
}

TEST_F(IterateSuite, JiggleTest) {
    auto its = sorted_chars.begin();
    auto itt = char_tree.begin();
    for (size_t i = 0; i < 26; ++i) {
        std::advance(its, i);
        std::advance(itt, i);
        EXPECT_EQ(*its, *itt);
        std::advance(its, -i);
        std::advance(itt, -i);
    }
    ASSERT_NE(itt, char_tree.end());
    std::advance(itt, 26);
    EXPECT_EQ(itt, char_tree.end());
    std::advance(itt, -26);
    EXPECT_EQ(itt, char_tree.begin());
}

TEST_F(IterateSuite, BeginEqEndEmpty) {
    b_tree::BTree<unsigned short, 3> tree;
    tree.insert(2);
    tree.remove(2);
    ASSERT_TRUE(tree.empty());
    EXPECT_TRUE(tree.begin() == tree.end());
}
