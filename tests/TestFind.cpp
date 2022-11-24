#include <string>

#include "gtest/gtest.h"
#include "b_tree.h"

class FindSuite : public testing::Test {
protected:
    b_tree::BTree<std::string, 3> tree{};

    size_t count_find(std::string const & v) {
        auto it = tree.find(v);
        size_t elements = 0;
        while (it != tree.end() && *it == v) {
            ++elements;
            ++it;
        }
        return elements;
    }
};

TEST_F(FindSuite, FindGetsFirst) {
    const std::string hate = "I hate B-trees";
    const std::string love = "I love B-trees";
    const std::string empty{};
    const std::string geq = "I >=3 B-trees";
    tree.insert(empty);
    for (size_t i = 0; i < 10; ++i) {
        tree.insert(hate);
        tree.insert(love);
    }
    tree.insert("I <3 B-trees");
    tree.insert(geq);
    tree.insert(geq);
    tree.insert("Angel");
    tree.insert("Hydra!");
    for (size_t i = 0; i < 41; ++i) {
        tree.insert(empty);
    }
    EXPECT_EQ(count_find(""), 42);
    EXPECT_EQ(count_find(love), 10);
    EXPECT_EQ(count_find(hate), 10);
    EXPECT_EQ(count_find("Angel"), 1);
    EXPECT_EQ(count_find("Angle"), 0);
    auto it = tree.find(geq);
    ASSERT_NE(it, tree.begin());
    EXPECT_FALSE(*--it == geq);
}
