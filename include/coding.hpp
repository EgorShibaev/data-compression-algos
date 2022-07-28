#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "huffman_tree.hpp"

namespace coding {

	class bit_reader {
		char cur;
		int offset;
		std::istream &in;
	public:
		static int read_byte_count;

		explicit bit_reader(std::istream &);

		bool read_bit();
	};

	class bit_writer {
		char cur;
		int offset;
		std::ostream &out;
	public:
		static int written_byte_count;

		explicit bit_writer(std::ostream &);

		~bit_writer();

		void write_bit(bool bit);
	};

	class encoder {
		std::string codes[256];

		int symbols_count;

		bit_writer writer;
	public:
		explicit encoder(const std::vector<std::string> &, int, std::ostream &);

		void encode(std::istream &in);
	};

	class decoder {
		huffman_tree::huffman_tree tree;

		int symbols_count;

		bit_reader reader;
	public:
		explicit decoder(huffman_tree::huffman_tree &&, int, std::istream &in);

		void decode(std::ostream &out);
	};

}