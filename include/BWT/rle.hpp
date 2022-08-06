#pragma once

#include <string>

namespace rle {

	std::string transform(const std::string &source);

	class inv_transformator {
		size_t block_size;
		std::string curr_string;

		char cur_count = 0;

	public:

		explicit inv_transformator(size_t block_size);

		bool is_ready() const;

		std::string get_string();

		void add_char(char);

	};
}

