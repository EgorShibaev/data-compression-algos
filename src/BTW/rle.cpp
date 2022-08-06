#include "BTW/rle.hpp"
#include <vector>
#include <iostream>

namespace rle {

	std::string transform(const std::string &source) {
		std::string res;
		int len = static_cast<int>(source.length());
		std::vector<int> type(len);
		int i = 0;
		while (i < len) {
			int j = i + 1;
			while (j < len && source[i] == source[j])
				++j;
			if (j - i >= 3)
				for (int k = i; k < j; ++k)
					type[k] = 1;
			else
				for (int k = i; k < j; ++k)
					type[k] = 0;
			i = j;
		}
		i = 0;
		while (i < len) {
			if (type[i]) {
				++i;
				continue;
			}
			int j = i + 1;
			while (j < len && !type[j])
				++j;
			bool ok = (j - i) % 2 == 0;
			for (int k = i; k < j; k += 2)
				if (source[k] != source[k + 1])
					ok = false;
			if (ok)
				for (int k = i; k < j; ++k)
					type[k] = 1;
			i = j;
		}
		i = 0;
		while (i < len) {
			int j = i + 1;
			while (j < len && type[j] == type[i] && (type[j] == 0 || source[j] == source[i]) && j - i < INT8_MAX)
				++j;
			if (type[i]) {
				res.push_back(static_cast<char>(j - i));
				res.push_back(source[i]);
			}
			else {
				res.push_back(static_cast<char>(-(j - i)));
				for (int k = i; k < j; ++k)
					res.push_back(source[k]);
			}
			i = j;
		}
		return res;
	}

	inv_transformator::inv_transformator(size_t block_size): block_size(block_size) {}

	bool inv_transformator::is_ready() const {
		return curr_string.length() == block_size;
	}

	std::string inv_transformator::get_string() {
		auto res = curr_string;
		curr_string = "";
		return res;
	}

	void inv_transformator::add_char(char ch) {
		if (cur_count > 0) {
			for (int i = 0; i < cur_count; ++i)
				curr_string.push_back(ch);
			cur_count = 0;
		}
		else if (cur_count < 0) {
			curr_string.push_back(ch);
			++cur_count;
		}
		else
			cur_count = ch;
	}
}