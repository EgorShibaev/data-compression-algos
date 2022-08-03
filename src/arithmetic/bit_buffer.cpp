#include "arithmetic/bit_buffer.hpp"
#include "arithmetic/range.hpp"

namespace bit_buffer {

	uint8_t bit_buffer::operator[](int i) const {
		return value[i];
	}

	void bit_buffer::add(uint8_t byte) {
		value.push_back(byte);
	}

	void bit_buffer::pop_front() {
		value.erase(value.cbegin());
	}

	uint8_t bit_buffer::front() const {
		return value[0];
	}

	size_t bit_buffer::size() const {
		return value.size() * 8;
	}

	void bit_buffer::show(std::ostream &out) {
		for (const auto& x: value)
			range::show_byte(x, out);
		out << "\n";
	}
}