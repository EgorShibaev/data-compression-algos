#include "arithmetic/bit_buffer.hpp"

namespace bit_buffer {

	bool bit_buffer::operator[](int i) const {
		return value[i];
	}

	void bit_buffer::add(bool bit) {
		value.push_back(bit);
	}

	void bit_buffer::pop_front() {
		value.erase(value.cbegin());
	}

	size_t bit_buffer::size() const {
		return value.size();
	}
}