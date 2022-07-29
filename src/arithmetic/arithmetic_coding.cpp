#include "arithmetic/arithmetic_coding.hpp"
#include "constants.hpp"

namespace arithmetic_coding {

	encoder::encoder(const statistic::statistic & stat, std::ostream &out, int symbol_count) :
			writer(out), symbols_count(symbol_count) {
		adjusted_statistic = stat;
		auto sum = adjusted_statistic.sum();
		int pow = 0;
		while ((1 << pow) < sum)
			++pow;
		stat_sum_pow = pow; // nearly sum
		for (int i = 0; i < bytes_count; ++i) {
			adjusted_statistic.set(i, (adjusted_statistic.get(i) * sum) / (1 << stat_sum_pow));
		}
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

			range.change_according_to_char(left, right, stat_sum_pow);

			while (range.new_bit()) {
				writer.write_bit(range.get_new_bit());
			}
		}
	}

}
