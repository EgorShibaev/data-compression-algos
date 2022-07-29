#include "huffman/huffman_coding.hpp"
#include "constants.hpp"
#include <iostream>

namespace huffman_coding {

	encoder::encoder(const std::vector<std::string> &codes_, int symbols_count, std::ostream &out) :
			symbols_count(symbols_count), writer(out) {
		for (int i = 0; i < bytes_count; ++i)
			codes[i] = codes_[i];
	}

	void encoder::encode(std::istream &in) {
		for (int i = 0; i < symbols_count; ++i) {
			char ch;
			in.get(ch);
			std::string code = codes[(unsigned char) ch];
			for (char bit: code)
				writer.write_bit(bit == '1');
		}
	}

	decoder::decoder(huffman_tree::huffman_tree &&tree, int symbols_count, std::istream &in) :
			tree(std::move(tree)), symbols_count(symbols_count), reader(in) {}


	void decoder::decode(std::ostream &out) {
		for (int i = 0; i < symbols_count; ++i) {
			huffman_tree::huffman_tree_node *cur = tree.root();
			while (!cur->is_leaf()) {
				if (reader.read_bit())
					cur = cur->right();
				else
					cur = cur->left();
			}
			out.put(cur->str()[0]);
		}
	}

}