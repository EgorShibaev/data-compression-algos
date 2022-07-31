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

	bool bit_buffer::front() const {
		return value[0];
	}

	size_t bit_buffer::size() const {
		return value.size();
	}

	void bit_buffer::show(std::ostream &out) {
		for (const auto& x: value)
			out << x;
		out << "\n";
	}
}