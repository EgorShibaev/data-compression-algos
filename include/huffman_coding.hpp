#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "huffman_tree.hpp"
#include "bit_manipulating.hpp"

namespace huffman_coding {

	class encoder {
		std::string codes[256];

		int symbols_count;

		bit_manipulating::bit_writer writer;
	public:
		explicit encoder(const std::vector<std::string> &, int, std::ostream &);

		void encode(std::istream &in);
	};

	class decoder {
		huffman_tree::huffman_tree tree;

		int symbols_count;

		bit_manipulating::bit_reader reader;
	public:
		explicit decoder(huffman_tree::huffman_tree &&, int, std::istream &in);

		void decode(std::ostream &out);
	};

}