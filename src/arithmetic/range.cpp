#include "arithmetic/range.hpp"
#include <iostream>
#include <cassert>

namespace range {

	range::exact_double::exact_double(bool value) : value{value} {}

	int range::exact_double::size() const {
		return static_cast<int>(value.size()) * 8;
	}

	uint8_t range::exact_double::front_byte() {
		if (value.empty())
			value.emplace_back(0);
		return value[0];
	}

	void range::exact_double::pop_front() {
		front_byte();
		value.erase(value.cbegin());
	}

	range::exact_double &range::exact_double::operator+=(const exact_double &other) {
		int next_pos = 0;
		while (value.size() < other.value.size())
			value.emplace_back(0);
		for (int i = static_cast<int>(other.value.size() - 1); i > -1; --i) {
			int sum = value[i] + other.value[i] + next_pos;
			value[i] = sum % 256;
			next_pos = sum / 256;
		}
		return *this;
	}

	range::exact_double &range::exact_double::operator-=(const exact_double &other) {
		int borrowed_one = 0;
		while (value.size() < other.value.size())
			value.emplace_back(0);
		for (int i = static_cast<int>(other.value.size() - 1); i > -1; --i) {
			int val = value[i] - borrowed_one;
			if (val < other.value[i]) {
				val += 256;
				borrowed_one = 1;
			}
			else
				borrowed_one = 0;
			value[i] = val - other.value[i];
		}
		return *this;
	}

	range::exact_double &range::exact_double::operator/=(int n) {
		int whole = n / 8;
		int shift = n % 8;

		int len = static_cast<int>(value.size());
		value.resize(value.size() + whole);
		for (int i = len - 1; i > -1; --i)
			value[i + whole] = value[i];
		for (int i = 0; i < whole; ++i)
			value[i] = 0;

		value.emplace_back(0);
		int last_byte = 0;
		for (auto & byte : value) {
			int first_bits = byte % (1 << shift);
			byte >>= shift;
			byte += last_byte << (8 - shift);
			last_byte = first_bits;
		}

		return *this;
	}

	range::exact_double &range::exact_double::operator*=(int other) {
		int next_pos = 0;
		for (int i = static_cast<int>(value.size() - 1); i > -1; --i) {
			int val = next_pos + value[i] * other;
			value[i] = val % 256;
			next_pos = val / 256;
		}
		return *this;
	}

	range::exact_double::Compare_result range::exact_double::compare(const bit_buffer::bit_buffer &buf,
																	 const std::deque<uint8_t> &common) {

		while (common.size() + value.size() < buf.size() / 8)
			value.emplace_back(0);
		for (int i = 0; i < static_cast<int>(buf.size() / 8); ++i) {
			auto val = i < static_cast<int>(common.size()) ? common[i] : value[i - common.size()];
			if (buf[i] != val) {
				return buf[i] > val ? Compare_result::less : Compare_result::more;
			}
		}
		return Compare_result::maybe_eq;
	}

	void show_byte(uint8_t byte, std::ostream& out) {
		for (int i = 0; i < 8; ++i)
			out << ((byte >> (7 - i)) & 1);
	}

	void range::exact_double::show(std::ostream &out, const std::deque<uint8_t> &common) {
		for (const auto& x: common)
			show_byte(x, out);
		for (const auto& x: value)
			show_byte(x, out);
		out << std::endl;
	}

	void range::exact_double::shrink_by_increasing() {
		shrink_by_zero_deleting();
		int max_size = 10; // later move to consts
		if (static_cast<int>(value.size()) > max_size) {
			int i = max_size;
			while (i < static_cast<int>(value.size()) && value[i] > 0)
				++i;
			value.resize(i + 1);
			value[i] = 1;
		}
	}

	void range::exact_double::shrink_by_decreasing() {
		shrink_by_zero_deleting();
		int max_size = 10; // later move to consts
		if (static_cast<int>(value.size()) > max_size) {
			value.resize(max_size);
		}
	}

	void range::exact_double::shrink_by_zero_deleting() {
		int len = static_cast<int>(value.size());
		while (len > 0 && value[len - 1] == 0)
			--len;
		value.resize(len);
	}

	range::range() : left(false), right(true) {}

	bool range::new_byte() {
		return (left.front_byte() == right.front_byte()) || !common_part.empty();
	}

	uint8_t range::get_new_byte() {
		if (left.front_byte() == right.front_byte()) {
			common_part.push_back(left.front_byte());
			left.pop_front();
			right.pop_front();
		}
		return common_part.front();
	}

	void range::pop_new_byte() {
		common_part.pop_front();
	}


	void range::change_according_to_char(int new_left, int new_right, int n) {
		auto dist = std::move(right);
		dist -= left;
		dist /= n;
		auto dist2 = dist;
		dist *= new_left;
		dist2 *= new_right;
		right = left;
		right += dist2;
		left += dist;
	}

	bool range::is_strictly_in(const bit_buffer::bit_buffer &val) {
		return left.compare(val, common_part) == exact_double::Compare_result::less &&
		       right.compare(val, common_part) == exact_double::Compare_result::more;
	}

	void range::choose_point() {
		auto l = left;
		auto r = right;
		l /= 1;
		r /= 1;
		l += r;
		left = l;
		right = l;

		auto add = exact_double(true);
		add /= right.size();
		right += add;
	}

	void range::show(std::ostream &out) {
		left.show(out, common_part);
		right.show(out, common_part);
	}

	void range::shrink() {
		left.shrink_by_increasing();
		right.shrink_by_decreasing();
	}

}