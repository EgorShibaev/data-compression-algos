#pragma once

#include <vector>
#include <iostream>

namespace bit_buffer {

	class bit_buffer {
		std::vector<uint8_t> value;

	public:

		uint8_t operator[](int i) const;

		void add(uint8_t byte);

		void pop_front();

		[[nodiscard]] uint8_t front() const;

		[[nodiscard]] size_t size() const;

		void show(std::ostream&);
	};
}
