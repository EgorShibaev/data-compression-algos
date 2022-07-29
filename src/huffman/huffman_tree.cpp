#include "huffman/huffman_tree.hpp"
#include <set>
#include <iostream>
#include "constants.hpp"

namespace huffman_tree {

	huffman_tree_node::huffman_tree_node(unsigned char ch, int32_t count) :
			left_(nullptr), right_(nullptr), count_(count), is_leaf_(true), str_(std::string(1, (char) ch)) {}

	huffman_tree_node::huffman_tree_node(huffman_tree_node *left, huffman_tree_node *right) :
			left_(left), right_(right), count_(left->count_ + right->count_), is_leaf_(false),
			str_(left->str_ + right->str_) {}

	bool operator<(const huffman_tree_node &a, const huffman_tree_node &b) {
		if (a.count() != b.count())
			return a.count() > b.count();
		return a.str() < b.str();
	}

	int huffman_tree_node::count() const {
		return count_;
	}

	bool huffman_tree_node::is_leaf() const {
		return is_leaf_;
	}

	const std::string &huffman_tree_node::str() const {
		return str_;
	}

	huffman_tree_node *huffman_tree_node::left() const {
		return left_;
	}

	huffman_tree_node *huffman_tree_node::right() const {
		return right_;
	}

	huffman_tree::huffman_tree(statistic::statistic stat) {
		auto cmp = [](huffman_tree_node *a, huffman_tree_node *b) {
			return (*b) < (*a);
		};
		std::set<huffman_tree_node *, decltype(cmp)> roots(cmp);
		for (int i = 0; i < bytes_count; ++i) {
			if (stat.get(i) != 0)
				roots.insert(new huffman_tree_node(i, stat.get(i)));
		}
		while (roots.size() > 1) {
			auto *left = *roots.begin();
			roots.erase(roots.begin());
			auto *right = *roots.begin();
			roots.erase(roots.begin());
			roots.insert(new huffman_tree_node(left, right));
		}
		if (roots.size() == 1)
			root_ = *roots.begin();
	}

	void huffman_tree::collect_codes(huffman_tree_node *cur, std::string &acc, std::vector<std::string> &codes) {
		if (cur->is_leaf()) {
			codes[(unsigned char) cur->str()[0]] = acc;
			return;
		}
		acc.push_back('0');
		collect_codes(cur->left(), acc, codes);
		acc[acc.size() - 1] = '1';
		collect_codes(cur->right(), acc, codes);
		acc.pop_back();
	}

	std::vector<std::string> huffman_tree::codes() {
		if (root_ == nullptr) {
			return std::vector<std::string>(bytes_count);
		}
		std::string acc;
		std::vector<std::string> codes(bytes_count);
		if (root_->is_leaf())
			return codes;
		collect_codes(root_, acc, codes);
		return codes;
	}

	huffman_tree::huffman_tree(huffman_tree &&other) noexcept: root_(other.root_) {
		other.root_ = nullptr;
	}

	huffman_tree_node *huffman_tree::root() const {
		return root_;
	}


	huffman_tree_node::~huffman_tree_node() {
		delete left();
		delete right();
	}

	huffman_tree::~huffman_tree() {
		delete root_;
	}

}