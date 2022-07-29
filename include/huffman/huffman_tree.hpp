#pragma once
#include <vector>
#include <ostream>
#include <istream>
#include <string>
#include "statistic.hpp"

namespace huffman_tree {

	class huffman_tree_node {
		huffman_tree_node *left_;
		huffman_tree_node *right_;

		int count_;

		bool is_leaf_;
		std::string str_;
	public:
		~huffman_tree_node();

		huffman_tree_node(unsigned char, int32_t);

		huffman_tree_node(huffman_tree_node *left, huffman_tree_node *right);

		huffman_tree_node(const huffman_tree_node &other);

		int count() const;

		bool is_leaf() const;

		const std::string &str() const;

		huffman_tree_node *left() const;

		huffman_tree_node *right() const;
	};

	bool operator<(const huffman_tree_node &a, const huffman_tree_node &b);

	class huffman_tree {
		void collect_codes(huffman_tree_node *, std::string &, std::vector<std::string> &);

		huffman_tree_node *root_ = nullptr;
	public:
		huffman_tree_node *root() const;

		~huffman_tree();

		huffman_tree(huffman_tree &&other) noexcept;

		explicit huffman_tree(statistic::statistic);

		std::vector<std::string> codes();
	};

}