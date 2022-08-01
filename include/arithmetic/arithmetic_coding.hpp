#pragma once

#include "range.hpp"
#include "statistic.hpp"
#include "bit_manipulating.hpp"
#include <iostream>

namespace arithmetic_coding {

	statistic::statistic adjust_stat(const statistic::statistic&);

	class encoder {
		range::range range;
		statistic::statistic adjusted_statistic;
		bit_manipulating::bit_writer writer;
		int symbols_count;
		int stat_sum_log; // pow of 2 that is adj_stat.sum()

	public:

		explicit encoder(const statistic::statistic&, std::ostream &);

		void encode(std::istream &);

	};

	class decoder {
		range::range range;
		statistic::statistic adjusted_statistic;
		int symbol_count;
		int stat_sum_log;
		std::istream& in;

	public:

		explicit decoder(const statistic::statistic&, std::istream &);

		void decode(std::ostream&);
	};

}