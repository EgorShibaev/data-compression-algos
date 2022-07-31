#include "arithmetic/arithmetic_coding.hpp"
#include "constants.hpp"
#include <iostream>

namespace arithmetic_coding {

	int my_log_2(int val) {
		int pow = 0;
		while ((1 << pow) < val)
			++pow;
		return pow;
	}

	statistic::statistic adjust_stat(const statistic::statistic &stat) {
		auto res = stat;
		auto sum = res.sum();
		int pow = my_log_2(sum);
		for (int i = 0; i < bytes_count; ++i) {
			res.set(i, (res.get(i) * (1 << pow)) / sum);
		}
		int curr_i = 0;
		int curr_sum = res.sum();
		while (curr_sum < (1 << pow)) {
			if (res.get(curr_i) != 0) {
				res.set(curr_i, res.get(curr_i) + 1);
				curr_sum++;
			}
			curr_i++;
			if (curr_i == bytes_count)
				curr_i = 0;
		}
		return res;
	}

	encoder::encoder(const statistic::statistic &stat, std::ostream &out) :
			adjusted_statistic(adjust_stat(stat)), writer(out), symbols_count(stat.sum()) {
		stat_sum_log = my_log_2(adjusted_statistic.sum());
	}

	void encoder::encode(std::istream &in) {
		int32_t prefix_sums[bytes_count + 1]{};
		for (int i = 0; i < bytes_count; ++i)
			prefix_sums[i + 1] = prefix_sums[i] + adjusted_statistic.get(i);

		for (int i = 0; i < symbols_count; ++i) {
			char ch;
			in.get(ch);

			int left = prefix_sums[static_cast<unsigned char>(ch)];
			int right = prefix_sums[static_cast<unsigned char>(ch) + 1];

			range.change_according_to_char(left, right, stat_sum_log);

			if (i == symbols_count - 1)
				range.choose_point();

			while (range.new_bit()) {
				writer.write_bit(range.get_new_bit());
				range.pop_new_bit();
			}

			range.shrink();
		}
	}


	decoder::decoder(const statistic::statistic &stat, std::istream &in) :
			adjusted_statistic(adjust_stat(stat)), reader(in), symbol_count(stat.sum()) {
		stat_sum_log = my_log_2(adjusted_statistic.sum());

	}

	int check_new_bit(range::range *next_ranges, const bit_buffer::bit_buffer& buf) {
		for (int i = 0; i < bytes_count; ++i)
			if (next_ranges[i].is_strictly_in(buf))
				return i;
		return -1;
	}

	void decoder::decode(std::ostream &out) {
		int32_t prefix_sums[bytes_count + 1]{};
		for (int i = 0; i < bytes_count; ++i)
			prefix_sums[i + 1] = prefix_sums[i] + adjusted_statistic.get(i);

		bit_buffer::bit_buffer buf;

		for (int i = 0; i < symbol_count; ++i) {
			range::range next_ranges[bytes_count];

			for (int ch = 0; ch < bytes_count; ++ch) {
				next_ranges[ch] = range;
				next_ranges[ch].change_according_to_char(
						prefix_sums[ch], prefix_sums[ch + 1], stat_sum_log);
			}

			int next_char = check_new_bit(next_ranges, buf);

			while (next_char == -1) {
				buf.add(reader.read_bit());
				next_char = check_new_bit(next_ranges, buf);
				// I know it is slow. Later I'll optimize it. Read blocks or smth.
			}

			out.put(static_cast<char>(static_cast<unsigned char>(next_char)));
			range = next_ranges[next_char];

			while (range.new_bit() && range.get_new_bit() == buf.front()) {
				range.pop_new_bit();
				buf.pop_front();
			}

			range.shrink();
		}
	}

}
