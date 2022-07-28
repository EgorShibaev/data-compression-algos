#include "coding.hpp"
#include "constants.hpp"
#include <iostream>

namespace coding {

	bit_reader::bit_reader(std::istream &in) : cur(0), offset(byte_size), in(in) {}

	int bit_reader::read_byte_count = 0;
	int bit_writer::written_byte_count = 0;

	bool bit_reader::read_bit() {
		if (offset == byte_size) {
			offset = 0;
			in.get(cur);
			bit_reader::read_byte_count++;
		}
		return (((unsigned char) cur) >> (offset++)) & 1;
	}

	bit_writer::bit_writer(std::ostream &out) : cur(0), offset(0), out(out) {}

	void bit_writer::write_bit(bool bit) {
		cur = (char) ((int) cur + ((1 << (offset++)) * bit));
		if (offset == byte_size) {
			out.put(cur);
			offset = 0;
			cur = 0;
			bit_writer::written_byte_count++;
		}
	}

	bit_writer::~bit_writer() {
		if (offset != 0) {
			out.put(cur);
			bit_writer::written_byte_count++;
		}
	}

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