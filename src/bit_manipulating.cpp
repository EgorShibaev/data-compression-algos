#include "bit_manipulating.hpp"
#include "constants.hpp"

namespace bit_manipulating {
	bit_reader::bit_reader(std::istream &in) : cur(0), offset(byte_size), in(in) {}

	bool bit_reader::read_bit() {
		if (offset == byte_size) {
			offset = 0;
			in.get(cur);
		}
		return (((unsigned char) cur) >> (offset++)) & 1;
	}

	bit_writer::bit_writer(std::ostream &out) : cur(0), offset(0), out(out) {}

	void bit_writer::write_bit(bool bit) {
		cur = (char) ((int) cur + ((1 << (offset++)) * bit));
		if (offset == byte_size) {
			out.put(cur);
			offset = 0;
			cur = 0;
		}
	}

	bit_writer::~bit_writer() {
		if (offset != 0) {
			out.put(cur);
		}
	}
}
