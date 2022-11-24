#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <stack>
#include <tuple>
#include <vector>

namespace b_tree {

template<std::copyable T, size_t Order, typename Comparator = std::less<>>
class BTree {
    static_assert(Order > 1, "Order must be greater than 1");

    static const size_t max_children = Order * 2;
    static const size_t max_keys = max_children - 1;
    static const size_t min_children = Order;
    static const size_t min_keys = min_children - 1;

    static_assert(min_children < max_children);
    static_assert(min_keys < max_keys);
    // Deletion uses this.
    static_assert(min_keys * 2 + 1 == max_keys);
    // Allows not to explicitly null children pointers when deleting
    // or splitting. To tell a node not to delete its child pointers,
    // setting its number of keys to 0 is needed. This doesn't allow
    // for an order 1 tree (effectively a binary tree), but using a
    // B-tree of such order would not be sensible anyway.
    static_assert(min_keys != 0);

    struct Node {
    private:
        Node() noexcept: keys_(new T[max_keys]), key_num_() {}

        explicit Node(T first_value) noexcept: Node() {  // initial root creation
            keys_[0] = first_value;
            ++key_num_;
        }

        explicit Node(Node *first_child) : Node() {  // new root creation
            assert(first_child != nullptr);
            children_[0] = first_child;
            split_child_right(0);
        }

        [[nodiscard]] bool is_full() const noexcept {
            return key_num_ == max_keys;
        }

        [[nodiscard]] bool is_leaf_node() const noexcept {
            return children_[0] == nullptr;
        };

        [[nodiscard]] bool is_internal_node() const noexcept {
            return children_[0] != nullptr;
        };

        void split_child_right(size_t index) {
            assert(key_num_ < max_keys);
            assert(is_internal_node());
            Node *child = children_[index];
            assert(child->key_num_ == max_keys);
            Node *new_child = new Node();

            const size_t new_keys = child->key_num_ / 2;
            for (size_t i = key_num_; i > index; --i) {
                children_[i + 1] = children_[i];
            }
            for (size_t i = key_num_; i > index; --i) {
                keys_[i] = keys_[i - 1];
            }
            keys_[index] = child->keys_[new_keys];
            ++key_num_;
            children_[index + 1] = new_child;

            const size_t offset = new_keys + 1;
            new_child->key_num_ = child->key_num_ - offset;
            for (size_t i = 0; i < new_child->key_num_; i++) {
                new_child->keys_[i] = child->keys_[i + offset];
            }
            child->key_num_ = new_keys;
            if (child->is_internal_node()) {
                for (size_t i = 0; i <= new_child->key_num_; i++) {
                    new_child->children_[i] = child->children_[i + offset];
                }
            }
        }

        void remove_leaf(size_t index) noexcept {
            assert(is_leaf_node());
            //assert(key_num_ > min_keys); may not hold for root
            for (size_t i = index + 1; i < key_num_; ++i) {
                keys_[i - 1] = keys_[i];
            }
            --key_num_;
        }

        void ensure_child_full(size_t index) noexcept {
            assert(index >= 0 && index <= key_num_);
            if (children_[index]->key_num_ > min_keys) return;
            assert(children_[index]->key_num_ == min_keys);
            if (index != 0 && children_[index - 1]->key_num_ > min_keys) {
                take_from_left(index);
            } else if (index != key_num_ && children_[index + 1]->key_num_ > min_keys) {
                take_from_right(index);
            } else {
                assert(index == 0
                       ? children_[1]->key_num_ == min_keys
                       : (index == key_num_ ? children_[key_num_ - 1]->key_num_ == min_keys
                                            : children_[index - 1]->key_num_ == min_keys
                                              && children_[index + 1]->key_num_ == min_keys));
                merge_child_with_right(std::min(index, key_num_ - 1));
            }
        }

        void merge_child_with_right(size_t index) noexcept {
            //assert(key_num_ > min_keys); // may not hold for root
            assert(index + 1 <= key_num_);
            assert(is_internal_node());
            Node *center_child = children_[index];
            Node *right_child = children_[index + 1];
            size_t const right_keys = right_child->key_num_;
            size_t const center_keys = center_child->key_num_;
            assert(right_keys == min_keys);
            assert(center_keys == min_keys);
            size_t const offset = center_keys + 1;

            center_child->keys_[center_keys] = keys_[index];
            for (size_t i = index + 1; i < key_num_; ++i) {
                keys_[i - 1] = keys_[i];
                children_[i] = children_[i + 1];
            }
            --key_num_;

            for (size_t i = 0; i < right_keys; ++i) {
                center_child->keys_[i + offset] = right_child->keys_[i];
            }
            if (center_child->is_internal_node()) {
                for (size_t i = 0; i <= right_keys; ++i) {
                    center_child->children_[i + offset] = right_child->children_[i];
                }
            }

            center_child->key_num_ += right_keys + 1;
            assert(center_child->key_num_ == max_keys);
            right_child->key_num_ = 0;
            delete right_child;
        }

        ~Node() {
            delete[] keys_;
            if (key_num_ == 0) return; // only possible when deleting an old root or merging
            for (size_t i = 0; i <= key_num_; ++i) {
                delete children_[i];
            }
        }

        Node *clone() const {
            assert(key_num_ != 0);
            auto node = new Node();
            for (size_t i = 0; i < key_num_; ++i) {
                node->keys_[i] = keys_[i];
            }
            node->key_num_ = key_num_;
            if (is_leaf_node()) return node;
            for (size_t i = 0; i <= key_num_; ++i) {
                node->children_[i] = children_[i]->clone();
            }
            return node;
        };

        void take_from_left(size_t index) noexcept {
            assert(index > 0);
            Node *center_child = children_[index];
            Node *left_child = children_[index - 1];
            assert(left_child->key_num_ > min_keys);
            assert(center_child->key_num_ == min_keys);

            for (size_t i = center_child->key_num_; i > 0; --i) {
                center_child->keys_[i] = center_child->keys_[i - 1];
            }
            center_child->keys_[0] = keys_[index - 1];
            if (center_child->is_internal_node()) {
                for (size_t i = center_child->key_num_ + 1; i > 0; --i) {
                    center_child->children_[i] = center_child->children_[i - 1];
                }
                center_child->children_[0] = left_child->children_[left_child->key_num_];
            }
            ++center_child->key_num_;

            --left_child->key_num_;
            keys_[index - 1] = left_child->keys_[left_child->key_num_];
        }

        void take_from_right(size_t index) noexcept {
            assert(index + 1 <= key_num_);
            Node *center_child = children_[index];
            Node *right_child = children_[index + 1];
            assert(right_child->key_num_ > min_keys);
            assert(center_child->key_num_ == min_keys);

            center_child->keys_[center_child->key_num_] = keys_[index];
            ++center_child->key_num_;
            keys_[index] = right_child->keys_[0];

            for (size_t i = 1; i < right_child->key_num_; ++i) {
                right_child->keys_[i - 1] = right_child->keys_[i];
            }
            if (right_child->is_internal_node()) {
                center_child->children_[center_child->key_num_] = right_child->children_[0];
                for (size_t i = 1; i <= right_child->key_num_; ++i) {
                    right_child->children_[i - 1] = right_child->children_[i];
                }
            }
            --right_child->key_num_;
        }

        Node *children_[max_children]{};
        T *keys_;
        size_t key_num_{};

        friend class BTree;
    };

    struct const_iterator {
        // Iteration is somewhat slow, but it's not like anyone moves
        // a reverse iterator around randomly when using a B-tree.
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T const;
        using pointer = value_type *;
        using reference = value_type &;

        const_iterator(const_iterator const &) = default;

        const_iterator &operator--() {
            iter_info const &last = state_.top();
            if (last.node->is_leaf_node()) {
                if (last.key_index == 0) {
                    while (state_.top().key_index == 0) {
                        state_.pop();
                    }
                }
            } else {
                Node *node = last.node->children_[last.key_index];
                while (node->is_internal_node()) {
                    state_.push({node, node->key_num_});
                    node = node->children_[node->key_num_];
                }
                state_.push({node, node->key_num_});
            }
            --state_.top().key_index;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator temp{*this};
            --(*this);
            return temp;
        }

        const_iterator &operator++() {
            iter_info &last = state_.top();
            ++last.key_index;
            if (last.node == fin_node_) {
                assert(fin_node_->is_leaf_node());
                return *this;
            }
            if (last.node->is_internal_node()) {
                Node *node = last.node->children_[last.key_index];
                while (node->is_internal_node()) {
                    state_.push({node, 0});
                    node = node->children_[0];
                }
                state_.push({node, 0});
            } else if (last.key_index == last.node->key_num_) {
                while (state_.top().key_index == state_.top().node->key_num_)
                    state_.pop();
            }
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator temp{*this};
            ++(*this);
            return temp;
        }

        reference operator*() const noexcept {
            iter_info const &info = state_.top();
            return info.node->keys_[info.key_index];
        }

        pointer operator->() const noexcept {
            iter_info const &info = state_.top();
            return info.node->keys_ + info.key_index;
        }

        friend bool operator==(const const_iterator &a, const const_iterator &b) noexcept {
            assert(a.state_.empty() == b.state_.empty());
            return a.state_.empty() && b.state_.empty()
            || a.state_.size() == b.state_.size() && a.state_.top().node == b.state_.top().node
               && a.state_.top().key_index == b.state_.top().key_index;
        }

        friend bool operator!=(const const_iterator &a, const const_iterator &b) noexcept {
            return !(a == b);
        }

    private:
        enum class TreePlace {
            // Avoid ambiguities with booleans (though using a boolean B-tree is silly).
            Begin,
            End,
        };

        const_iterator(BTree const &tree, T const &value) : fin_node_(get_last_node(tree.root_)) {
            Node *node = tree.root_;
            while (node != nullptr) {
                size_t index = tree.find_index(node, value);
                state_.push({node, index});
                if (index != node->key_num_ && tree.equals(node->keys_[index], value)) {
                    node = node->children_[index];
                    while (node != nullptr && (index = tree.find_index(node, value)) != node->key_num_) {
                        state_.push({node, index});
                        node = node->children_[index];
                    }
                    return;
                }
                node = node->children_[index];
            }
            state_ = std::move(tree.end().state_);
        }

        const_iterator(BTree const &tree, TreePlace place) : fin_node_(get_last_node(tree.root_)) {
            if (tree.root_ == nullptr) return;
            Node *node = tree.root_;
            if (place == TreePlace::Begin) {
                while (!node->is_leaf_node()) {
                    state_.push({node, 0});
                    node = node->children_[0];
                }
                state_.push({node, 0});
            } else {
                while (node->is_internal_node()) {
                    state_.push({node, node->key_num_});
                    node = node->children_[node->key_num_];
                }
                assert(node == fin_node_);
                state_.push({node, node->key_num_});
            }
        }

        [[nodiscard]] const Node *get_last_node(const Node *node) const {
            if (node == nullptr) return nullptr;
            while (node->is_internal_node())
                node = node->children_[node->key_num_];
            return node;
        };

        struct iter_info {
            Node *node;
            size_t key_index;
        };

        std::stack<iter_info> state_{};
        const Node *fin_node_;

        friend class BTree;
    };

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    BTree() = default;

    explicit BTree(Comparator comparator) : comparator_(comparator) {}

    BTree(const BTree &other) : root_(other.root_ == nullptr ? nullptr : other.root_->clone()),
                                comparator_(other.comparator_) {}

    BTree &operator=(const BTree &other) {
        if (this != &other) {
            BTree(other).swap(*this);
        }
        return *this;
    }

    BTree(BTree &&other) noexcept {
        swap(other);
    }

    BTree &operator=(BTree &&other) noexcept {
        swap(other);
        return *this;
    }

    void swap(BTree &other) noexcept {
        std::swap(root_, other.root_);
        std::swap(comparator_, other.comparator_);
    }

    [[nodiscard]] bool empty() const {
        return root_ == nullptr;
    }

    const_iterator find(const T &value) const {
        return const_iterator(*this, value);
    }

    bool contains(const T &value) const noexcept {
        Node *cur_node = root_;
        while (cur_node != nullptr) {
            size_t index = find_index(cur_node, value);
            if (index != cur_node->key_num_ && equals(cur_node->keys_[index], value)) {
                return true;
            }
            cur_node = cur_node->children_[index];
        }
        return false;
    }

    void insert(const T &value) {
        if (root_ == nullptr) {
            root_ = new Node(value);
            return;
        }

        if (root_->is_full()) {
            root_ = new Node(root_);
        }

        Node *cur_node = root_;
        while (cur_node->is_internal_node()) {
            cur_node = get_insertion_child(cur_node, value);
        }
        insert_leaf(cur_node, value);
    }

    void remove(T const &value) {
        if (root_ == nullptr) return;

        if (root_->key_num_ == 1) {
            if (root_->is_leaf_node()) {
                if (equals(root_->keys_[0], value)) {
                    delete root_;
                    root_ = nullptr;
                }
                return;
            } else if (root_->children_[0]->key_num_ == min_keys && root_->children_[1]->key_num_ == min_keys) {
                root_->merge_child_with_right(0);
                Node *old_root = root_;
                root_ = old_root->children_[0];
                delete old_root;
                assert(root_->key_num_ > 1);
            }
        }

        Node *cur_node = root_;
        size_t index = find_index(cur_node, value);
        while (cur_node->is_internal_node()) {
            assert(cur_node == root_ || cur_node->key_num_ > min_keys);
            if (index < cur_node->key_num_ && equals(cur_node->keys_[index], value)) {
                auto const &left_child = cur_node->children_[index];
                auto const &right_child = cur_node->children_[index + 1];
                if (left_child->key_num_ > min_keys) {
                    move_predecessor(left_child, cur_node->keys_[index]);
                    return;
                }
                if (right_child->key_num_ > min_keys) {
                    move_successor(right_child, cur_node->keys_[index]);
                    return;
                }
                assert(left_child->key_num_ == min_keys);
                assert(right_child->key_num_ == min_keys);
                //           0 ->4<- 8 ...
                //              / \
                // 1 2 3 _ _ _ _   5 6 7 _ _ _ _
                cur_node->merge_child_with_right(index);
                //                   0 8 ...
                //                  / \
                // 1 2 3 ->4<- 5 6 7    (right is deleted)
                remove_middle_key(left_child);
                return;
            }
            cur_node->ensure_child_full(index);
            cur_node = cur_node->children_[std::min(index, cur_node->key_num_)];
            index = find_index(cur_node, value);
        }

        if (index < cur_node->key_num_ && equals(cur_node->keys_[index], value)) {
            assert(cur_node == root_ || cur_node->key_num_ > min_keys);
            cur_node->remove_leaf(index);
        }
        // else not in tree
    }

    void clear() {
        delete root_;
        root_ = nullptr;
    }

    const_iterator begin() const { return const_iterator(*this, const_iterator::TreePlace::Begin); }

    const_iterator end() const { return const_iterator(*this, const_iterator::TreePlace::End); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    ~BTree() {
        delete root_;
    }

private:
    Node *root_ = nullptr;
    Comparator comparator_{};

    void remove_middle_key(Node *cur_node) {
        while (cur_node->is_internal_node()) {
            size_t const middle = cur_node->key_num_ / 2;
            auto const &left_child = cur_node->children_[middle];
            auto const &right_child = cur_node->children_[middle + 1];
            if (left_child->key_num_ > min_keys) {
                move_predecessor(left_child, cur_node->keys_[middle]);
                return;
            }
            if (right_child->key_num_ > min_keys) {
                move_successor(right_child, cur_node->keys_[middle]);
                return;
            }
            cur_node->merge_child_with_right(middle);
            cur_node = left_child;
        }
        cur_node->remove_leaf(cur_node->key_num_ / 2);
    }

    void move_predecessor(Node *node, T &move_to) {
        while (node->is_internal_node()) {
            node->ensure_child_full(node->key_num_);
            node = node->children_[node->key_num_];
        }
        move_to = node->keys_[node->key_num_ - 1];
        assert(node->key_num_ > min_keys);
        --node->key_num_;
    }

    void move_successor(Node *node, T &move_to) {
        while (node->is_internal_node()) {
            node->ensure_child_full(0);
            node = node->children_[0];
        }
        move_to = node->keys_[0];
        assert(node->key_num_ > min_keys);
        --node->key_num_;
        for (size_t i = 0; i < node->key_num_; ++i) {
            node->keys_[i] = node->keys_[i + 1];
        }
    }

    Node *get_insertion_child(Node *node, const T &value) {  // D:
        size_t index = find_index(node, value);
        if (node->children_[index]->is_full()) {
            node->split_child_right(index);
            if (comparator_(node->keys_[index], value)) ++index;
        }
        return node->children_[index];
    }

    void insert_leaf(Node *node, const T &value) {
        size_t index = find_index(node, value);
        for (size_t i = node->key_num_; i > index; --i) {
            node->keys_[i] = node->keys_[i - 1];
        }
        ++node->key_num_;
        node->keys_[index] = value;
    }

    size_t find_index(Node const *node, T const &value) const noexcept(
    noexcept(std::declval<Comparator>()(std::declval<T>(), std::declval<T>()))
    ) {
        // Returns index of the first occurrence of value or of the first
        // element greater than it
        // 2: 0 1 ->3<- 3 4 5
        // 2: 0 1 ->2<- 2 3 5
        T const *left = node->keys_;
        T const *right = left + node->key_num_;
        while (right != left) {
            T const *mid = left + (right - left) / 2;
            if (comparator_(*mid, value)) left = mid + 1;
            else right = mid;
        }
        return left - node->keys_;
    }

    bool equals(T const &a, T const &b) const noexcept(
    noexcept(std::declval<Comparator>()(std::declval<T>(), std::declval<T>()))
    ) {
        // account for different comparators
        return comparator_(a, b) == comparator_(b, a);
    }
};

template<std::copyable T, size_t Order, typename Comparator>
void swap(BTree<T, Order, Comparator> bTree1, BTree<T, Order, Comparator> bTree2) {
    bTree1.swap(bTree2);
}

}  // namespace b_tree
