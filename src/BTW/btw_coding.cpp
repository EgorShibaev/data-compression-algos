#include "BTW/btw_coding.hpp"
#include "BTW/btw.hpp"
#include "BTW/rle.hpp"
#include "statistic.hpp"
#include <cassert>
#include <algorithm>

namespace btw_coding {

	const size_t block_size = 100000;

	void write_block(std::ostream& out, const std::string& cur) {
		auto btw_tr = btw::transform(cur);

		assert(btw::inverse_transformation(btw_tr.first, btw_tr.second) == cur);
		int pos = btw_tr.second;
		auto rle_tr = rle::transform(btw_tr.first);
		statistic::write_int32_t(out, pos);
		out << rle_tr;
	}

	void encode(std::ostream& out, std::istream& in) {
		std::string cur;

		char ch;
		in.get(ch);

		int cnt = 0;

		while (!in.eof()) {
			cur.push_back(ch);
			if (cur.size() == block_size) {
				write_block(out, cur);
				++cnt;
				cur = "";
			}
			in.get(ch);
		}

		if (!cur.empty())
			write_block(out, cur);
	}

	void decode(std::ostream& out, std::istream& in) {
		rle::inv_transformator inv_tr(block_size);

		int pos = statistic::read_int32_t(in);
		char ch;
		in.get(ch);

		int cnt = 0;

		while (!in.eof()) {

			inv_tr.add_char(ch);
			if (inv_tr.is_ready()) {
				cnt++;
				auto btw_tr_str = inv_tr.get_string();
				auto source = btw::inverse_transformation(btw_tr_str, pos);
				out << source;

				pos = statistic::read_int32_t(in);
			}

			in.get(ch);

		}

		auto btw_tr_str = inv_tr.get_string();
		if (!btw_tr_str.empty()) {
			auto source = btw::inverse_transformation(btw_tr_str, pos);
			out << source;
		}
	}
}