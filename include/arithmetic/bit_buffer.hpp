#pragma once

#include <vector>
#include <iostream>

namespace bit_buffer {

	class bit_buffer {
		std::vector<bool> value;

	public:

		bool operator[](int i) const;

		void add(bool bit);

		void pop_front();

		[[nodiscard]] bool front() const;

		[[nodiscard]] size_t size() const;

		void show(std::ostream&);
	};
}
