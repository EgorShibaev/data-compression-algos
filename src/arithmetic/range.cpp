#include "arithmetic/range.hpp"
#include <iostream>
#include <cassert>

namespace range {

	range::exact_double::exact_double(bool value) : value{value} {}

	int range::exact_double::size() const {
		return static_cast<int>(value.size());
	}

	bool range::exact_double::front_bit() {
		if (value.empty())
			value.emplace_back(false);
		return value[0];
	}

	void range::exact_double::pop_front() {
		front_bit();
		value.erase(value.cbegin());
	}

	range::exact_double &range::exact_double::operator+=(const exact_double &other) {
		int next_bit = 0;
		while (value.size() < other.value.size())
			value.emplace_back(false);
		for (int i = static_cast<int>(other.value.size() - 1); i > -1; --i) {
			int sum = value[i] + other.value[i] + next_bit;
			value[i] = sum & 1;
			next_bit = sum >> 1;
		}
		return *this;
	}

	range::exact_double &range::exact_double::operator-=(const exact_double &other) {
		int borrowed_bit = 0;
		while (value.size() < other.value.size())
			value.emplace_back(false);
		for (int i = static_cast<int>(other.value.size() - 1); i > -1; --i) {
			int val = value[i] - borrowed_bit;
			if (val < other.value[i]) {
				val += 2;
				borrowed_bit = 1;
			}
			else
				borrowed_bit = 0;
			value[i] = val - other.value[i];
		}
		return *this;
	}

	range::exact_double &range::exact_double::operator/=(int n) {
		for (int i = 0; i < n; ++i)
			value.insert(value.cbegin(), false);
		return *this;
	}

	range::exact_double &range::exact_double::operator*=(int other) {
		int next_bit = 0;
		for (int i = static_cast<int>(value.size() - 1); i > -1; --i) {
			int val = next_bit + value[i] * other;
			value[i] = val & 1;
			next_bit = val >> 1;
		}
		return *this;
	}

	range::exact_double::Compare_result range::exact_double::compare(const bit_buffer::bit_buffer &buf,
																	 const std::deque<bool> &common) {

		while (common.size() + value.size() < buf.size())
			value.emplace_back(false);
		for (int i = 0; i < static_cast<int>(buf.size()); ++i) {
			auto val = i < static_cast<int>(common.size()) ? common[i] : value[i - common.size()];
			if (buf[i] != val) {
				return buf[i] > val ? Compare_result::less : Compare_result::more;
			}
		}
		return Compare_result::maybe_eq;
	}

	void range::exact_double::show(std::ostream &out, const std::deque<bool> &common) {
		for (const auto& x: common)
			out << x;
		for (const auto& x: value)
			out << x;
		out << std::endl;
	}

	range::exact_double range::exact_double::smallest_more() {
		assert(!front_bit());
		exact_double res(false);
		exact_double addition(true);
		int i = 1;
		while (i < static_cast<int>(value.size()) && value[i]) {
			++i;
			addition /= 1;
			res += addition;
		}
		addition /= 1;
		res += addition;

		return res;
	}

	void range::exact_double::shrink_by_increasing() {
		shrink_by_zero_deleting();
		int max_size = 50; // later move to consts
		if (static_cast<int>(value.size()) > max_size) {
			int i = max_size;
			while (i < static_cast<int>(value.size()) && value[i])
				++i;
			value.resize(i + 1);
			value[i] = true;
		}
	}

	void range::exact_double::shrink_by_decreasing() {
		shrink_by_zero_deleting();
		int max_size = 50; // later move to consts
		if (static_cast<int>(value.size()) > max_size) {
			value.resize(max_size);
		}
	}

	void range::exact_double::shrink_by_zero_deleting() {
		while (!value.empty() && !value[value.size() - 1])
			value.pop_back();
	}

	range::range() : left(false), right(true) {}

	bool range::new_bit() {
		return (left.front_bit() == right.front_bit()) || !common_part.empty();
	}

	bool range::get_new_bit() {
		if (left.front_bit() == right.front_bit()) {
			common_part.push_back(left.front_bit());
			left.pop_front();
			right.pop_front();
		}
		return common_part.front();
	}

	void range::pop_new_bit() {
		common_part.pop_front();
	}


	void range::change_according_to_char(int new_left, int new_right, int n) {
		auto dist = right;
		dist -= left;
		dist /= n;
		dist *= new_left;
		auto next_left = left;
		next_left += dist;
		dist = right;
		dist -= left;
		dist /= n;
		dist *= new_right;
		auto next_right = left;
		next_right += dist;
		left = next_left;
		right = next_right;

//		std::cout << left.size() << "\n";
//		std::cout << right.size() << "\n";
//		left.show(std::cout, {});
//		right.show(std::cout, {});
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