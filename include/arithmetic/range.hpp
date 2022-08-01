#pragma once

#include "bit_buffer.hpp"
#include <vector>
#include <deque>
#include <iostream>

namespace range {

	class range {

		class exact_double {
			std::vector<bool> value; // start with ones - 0.1101 saved as {0, 1, 1, 0, 1};

		public:

			explicit exact_double(bool value);

			bool front_bit();

			void pop_front();

			exact_double& operator+=(const exact_double&);

			exact_double& operator-=(const exact_double&);

			exact_double& operator*=(int value);

			exact_double& operator/=(int n); // div by 2 ^ n;

			enum class Compare_result {
				less, more, maybe_eq
			};

			Compare_result compare(const bit_buffer::bit_buffer&, const std::deque<bool>&);

			[[nodiscard]] int size() const;

			void shrink_by_zero_deleting();

			void shrink_by_increasing();

			void shrink_by_decreasing();

			void show(std::ostream&, const std::deque<bool>&);
		};

		std::deque<bool> common_part;
		exact_double left;
		exact_double right;

	public:

		range();

		bool get_new_bit();

		void pop_new_bit();

		bool new_bit();

		void change_according_to_char(int new_left, int new_right, int n); // 2^n - sum by all stat

		bool is_strictly_in(const bit_buffer::bit_buffer&);

		void choose_point();

		void shrink();

		void show(std::ostream&);
	};


}