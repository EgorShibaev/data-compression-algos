#include <BTW/btw.hpp>

#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
#include "constants.hpp"

namespace btw {

	std::vector<int> suffix_array(const std::string &str) {
		int len = static_cast<int>(str.length());

		std::vector<int> res(len);
		std::vector<int> classes(len);
		std::vector<int> cnt(bytes_count);

		for (int i = 0; i < len; ++i)
			++cnt[static_cast<unsigned char>(str[i])];
		for (int i = 1; i < bytes_count; ++i)
			cnt[i] += cnt[i - 1];
		for (int i = 0; i < len; ++i)
			res[--cnt[static_cast<unsigned char>(str[i])]] = i;

		int cl_num = 0;
		classes[res[0]] = 0;
		for (int i = 1; i < len; ++i) {
			if (str[res[i]] != str[res[i - 1]])
				++cl_num;
			classes[res[i]] = cl_num;
		}

		for (int stage = 0; (1 << stage) < len; ++stage) { // substring length 2^stage have already been sorted
			std::vector<int> new_res(len);
			for (int i = 0; i < len; ++i) {
				new_res[i] = res[i] - (1 << stage);
				if (new_res[i] < 0)
					new_res[i] += len;
			}

			cnt.assign(len, 0);
			for (int i = 0; i < len; ++i)
				++cnt[classes[new_res[i]]];
			for (int i = 1; i < len; ++i)
				cnt[i] += cnt[i - 1];
			for (int i = len - 1; i > -1; --i)
				res[--cnt[classes[new_res[i]]]] = new_res[i];

			// recount classes
			std::vector<int> new_classes(len);
			new_classes[res[0]] = 0;
			int classes_count = 1;
			for (int i = 1; i < len; ++i) {
				if (classes[res[i]] != classes[res[i - 1]] ||
				    classes[(res[i] + (1 << stage)) % len] != classes[(res[i - 1] + (1 << stage)) % len])
					++classes_count;
				new_classes[res[i]] = classes_count - 1;
			}
			classes = new_classes;
		}

		return res;
	}

	std::pair<std::string, int> transform(const std::string &initial) {
		int len = static_cast<int>(initial.length());

		auto suf_array = suffix_array(initial);
		std::string res(len, 0);

		for (int i = 0; i < len; ++i)
			res[i] = initial[(suf_array[i] - 1 + len) % len];

		int initial_pos = 0;
		for (int i = 0; i < len; ++i)
			if (suf_array[i] == 0)
				initial_pos = i;

		return std::make_pair(res, initial_pos);
	}

	std::string inverse_transformation(const std::string &transformed, int pos) {
		int len = static_cast<int>(transformed.length());

		std::vector<int> move(len);
		int cnt[bytes_count]{};
		for (int i = 0; i < len; ++i)
			++cnt[static_cast<unsigned char>(transformed[i])];

		for (int i = 1; i < bytes_count; ++i)
			cnt[i] += cnt[i - 1];

		for (int i = len - 1; i > -1; --i)
			move[i] = --cnt[static_cast<unsigned char>(transformed[i])];

		std::vector<int> back_move(len);

		for (int i = 0; i < len; ++i)
			back_move[move[i]] = i;

		std::string res;

		std::string col = transformed;
		std::sort(col.begin(), col.end(),
		          [](char ch1, char ch2) {
			          return static_cast<unsigned char>(ch1) < static_cast<unsigned char>(ch2);
		          });

		res.push_back(col[pos]);

		int cur_pos = pos;

		for (int i = 1; i < len; ++i) {
			cur_pos = back_move[cur_pos];
			res.push_back(col[cur_pos]);
		}

		return res;
	}

}