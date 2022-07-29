#pragma once

#include "range.hpp"
#include "statistic.hpp"
#include "bit_manipulating.hpp"
#include <iostream>

namespace arithmetic_coding {

	class encoder {

		range::range range;
		statistic::statistic adjusted_statistic;
		bit_manipulating::bit_writer writer;
		int symbols_count;
		int stat_sum_pow; // pow of 2 that is nearly adj_stat.sum()

	public:

		explicit encoder(const statistic::statistic&, std::ostream &, int sybmols_count);

		void encode(std::istream &);

	};

	class decoder {

	};

}