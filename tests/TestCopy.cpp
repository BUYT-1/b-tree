#include <vector>

#include "gtest/gtest.h"
#include "b_tree.h"

template<typename ElType, size_t Order, typename ComparatorType = std::less<>>
struct BTreeComparator {
    bool operator()(b_tree::BTree<ElType, Order, ComparatorType> a, b_tree::BTree<ElType, Order, ComparatorType> b)
    const {
        ComparatorType comparator{};
        auto ita = a.begin();
        auto itb = b.begin();
        while (ita != a.end() && itb != b.end()) {
            bool altb = comparator(*ita, *itb);
            bool blta = comparator(*itb, *ita);
            if (altb) {
                if (!blta) {
                    return true;
                }
            } else if (blta) {
                return false;
            }
            ++ita;
            ++itb;
        }
        if (ita == a.end() && itb != b.end()) {
            return true;
        }
        return false;
    }
};

TEST(CopySuite, CopyShort) {
    b_tree::BTree<short, 2> tree{};
    for (short i = -5; i < 59; ++i) {
        tree.insert(i);
    }
    auto copy = tree;
    for (short i = -5; i < 59; ++i) {
        EXPECT_TRUE(copy.contains(i));
    }
    EXPECT_FALSE(copy.contains(-7));
}

TEST(CopySuite, CopyString) {
    std::vector<std::string> quote{"World", "indignities", "made", "been", "and", "have", "have", "to", "suffering",
                                   "to", "to", "worsen", "continued", "us", "who", "may", "world.", "consequences",
                                   "Industrial", "human", "They", "damage", "on", "The", "suffering,", "those", "of",
                                   "and", "life-expectancy", "subject", "widespread", "greater", "destabilized",
                                   "countries,", "indignities,", "the", "the", "lead", "they", "suffering", "the",
                                   "human", "of", "well)", "and", "in", "have", "The", "natural", "countries.",
                                   "disruption", "of", "inflicted", "have", "greater", "will", "as", "have",
                                   "“advanced”", "have", "physical", "led", "natural", "subjected", "development",
                                   "inflict", "greatly", "human", "to", "its", "severe", "increased", "the", "will",
                                   "physical", "(in", "on", "suffering", "it", "It", "race.", "have", "a", "society,",
                                   "to", "beings", "Third", "to", "lead", "life", "even", "unfulfilling,", "Revolution",
                                   "psychological", "will", "increased", "disaster", "probably", "certainly", "live",
                                   "beings", "and", "damage", "in", "the", "the", "situation.", "technology",
                                   "“advanced”", "for", "greater", "it", "and", "but", "social", "world,",
                                   "psychological"};
    b_tree::BTree<std::string, 3> tree{};
    for (auto const &word: quote) {
        tree.insert(word);
    }
    auto copy = tree;
    for (auto const &word: quote) {
        EXPECT_TRUE(copy.contains(word));
    }
    EXPECT_FALSE(copy.contains("hopes"));
    EXPECT_FALSE(copy.contains("an"));
    EXPECT_FALSE(copy.contains("dreams"));
}

TEST(CopySuite, CopyBTreeString) {
    using TreeType = b_tree::BTree<std::string, 4>;
    using MainTreeType = b_tree::BTree<TreeType, 2, BTreeComparator<std::string, 4>>;
    MainTreeType main_tree;
    std::vector<TreeType> trees;
    for (size_t i = 0; i < 6; ++i) {
        trees.emplace_back();
    }
    std::vector<std::string> words{"the", "got", "Maple,", "said,", "and", "always", "her", "were", "be", "their",
                                   "headed", "their", "boss,", "that,”", "to", "but", "had", "and", "busy", "“She’ll",
                                   "she", "to", "and", "on", "chance", "items", "twice.", "for", "right.”", "boss.",
                                   "opening", "never", "for", "downed", "If", "to", "arguably", "Yui", "items", "this",
                                   "see", "good", "sure", "here.", "another", "She", "had", "into", "he’d", "ran",
                                   "They", "they", "ready", "job", "game.", "path", "bombs.", "get", "Chrome",
                                   "started", "“Totally", "deploy", "figure", "easily", "the", "in", "even", "parry",
                                   "dat.”", "potions,", "MP", "can’t", "tank", "leading", "with", "shower", "step",
                                   "need", "could", "it", "anything,", "thrilled", "“I’m", "floor,", "Mai", "a",
                                   "them.", "celebrating.", "may", "so", "chance", "Kasumi’s", "numbers,", "definitely",
                                   "gold", "stopping", "it", "not", "we", "time—but", "in", "she", "couldn’t", "to",
                                   "was", "next", "anything", "Pre-battle,", "my", "easily,", "do", "own", "Since",
                                   "did", "were", "burst", "change", "“True", "have", "gonna", "Maple’s", "the", "the",
                                   "boss", "to", "have", "good", "they", "and", "crafting", "in", "The", "“Still,",
                                   "post-battle,", "and", "shone", "battle,", "a", "said.", "done", "won", "anything.",
                                   "out.", "hope", "shine", "fights", "role", "unique.", "’cause", "their", "in", "of",
                                   "making", "the", "make", "crumpled", "was", "her", "used,”", "something", "still",
                                   "I", "out", "her", "the", "“Let’s", "they", "not", "progress.”", "up", "of",
                                   "Kasumi", "specced", "could", "and", "convert", "long.”", "restock", "Kasumi",
                                   "anywhere,", "never", "a", "items,", "chance.", "it’s", "Chrome", "alone.", "into",
                                   "and", "I", "Like", "light.", "if", "Iz", "buff", "were", "but", "twins", "we",
                                   "twins", "All", "guarding", "to", "way.”", "workshop", "Chrome", "workshop.", "a",
                                   "pulverize", "boss’s", "strikes", "“Whew…they", "was", "guild"};
    for (size_t i = 0; i < words.size(); ++i) {
        trees[i % trees.size()].insert(words[i]);
    }
    for (auto const& tree : trees) {
        main_tree.insert(tree);
    }
    MainTreeType copy{};
    copy = main_tree;
    for (auto const& tree : trees) {
        EXPECT_TRUE(copy.contains(tree));
    }
    MainTreeType copy2{copy};
    copy2.insert({});
    for (auto const& tree : trees) {
        EXPECT_TRUE(copy2.contains(tree));
    }
    EXPECT_TRUE(copy2.contains({}));
    EXPECT_FALSE(main_tree.contains({}));
}
