#include <iostream>
#include <filesystem>
#include "b_tree.h"

int main() {
    b_tree::BTree<size_t, 100> big_tree;
    for (size_t i = 0; i < 4312; ++i) {
        big_tree.insert(i);
    }
    std::cout << big_tree.contains(1) << std::endl;
    std::cout << big_tree.contains(4312) << std::endl;
    std::cout << big_tree.contains(4311) << std::endl;
    std::cout << big_tree.contains(1000) << std::endl;
    for (size_t i = 0; i < 4312; ++i) {
        big_tree.remove(i);
    }
    std::cout << big_tree.contains(1000) << std::endl;
    std::cout << big_tree.contains(1) << std::endl;
    std::cout << std::endl;

    b_tree::BTree<std::string, 2> string_tree;
    string_tree.insert("abc");
    string_tree.insert("abc");
    string_tree.insert("abc");
    string_tree.insert("abc");
    string_tree.insert("abc");
    string_tree.insert("abc");
    string_tree.insert("abd");
    string_tree.insert("aadba");
    string_tree.remove("abc");

    std::cout << string_tree.contains("abc") << std::endl;
    std::cout << string_tree.contains("ab") << std::endl;
    std::cout << string_tree.contains("abcd") << std::endl;

    std::cout << *string_tree.begin() << std::endl;
    std::cout << *--string_tree.end() << std::endl;
    std::cout << "in sorted order:" << std::endl;
    for (const auto& el : string_tree) {
        std::cout << el << std::endl;
    }
    std::cout << "in reverse order:" << std::endl;
    for (auto it = string_tree.rbegin(); it != string_tree.rend(); ++it) {
        std::cout << *it << std::endl;
    }

    return 0;
}
