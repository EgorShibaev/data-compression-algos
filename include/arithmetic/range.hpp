#pragma once

#include <vector>
#include <deque>

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
		};

		std::deque<bool> common_part;
		exact_double left;
		exact_double right;

	public:

		range();

		bool get_new_bit();

		bool new_bit();

		void change_according_to_char(int new_left, int new_right, int n); // 2^n - sum by all stat
	};

}