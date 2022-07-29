#pragma once

#include <vector>

namespace bit_buffer {

	class bit_buffer {
		std::vector<bool> value;

	public:

		bool operator[](int i) const;

		void add(bool bit);

		void pop_front();

		size_t size() const;

	};
}
