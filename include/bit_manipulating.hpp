#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace bit_manipulating {

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

}