#include "arithmetic/range.hpp"

namespace range {

	range::exact_double::exact_double(bool value): value{value} {}

	bool range::exact_double::front_bit() {
		if (value.empty())
			value.emplace_back(false);
		return value[0];
	}

	void range::exact_double::pop_front() {
		front_bit();
		value.erase(value.cbegin());
	}

	range::exact_double& range::exact_double::operator+=(const exact_double& other) {
		int next_bit = 0;
		while (value.size() < other.value.size())
			value.emplace_back(false);
		for (int i = static_cast<int>(other.value.size() - 1); i > - 1; --i) {
			int sum =  value[i] + other.value[i] + next_bit;
			value[i] = sum & 1;
			next_bit = sum >> 1;
		}
		return *this;
	}

	range::exact_double& range::exact_double::operator-=(const exact_double& other) {
		int borrowed_bit = 0;
		while (value.size() < other.value.size())
			value.emplace_back(false);
		for (int i = static_cast<int>(other.value.size() - 1); i > - 1; --i) {
			int val = value[i] - borrowed_bit;
			if (val < other.value[i]) {
				val += 2;
				borrowed_bit = 1;
			}
			value[i] = val - other.value[i];
		}
		return *this;
	}

	range::exact_double& range::exact_double::operator/=(int n) {
		for (int i = 0; i < n; ++i)
			value.insert(value.cbegin(), false);
		return *this;
	}

	range::exact_double& range::exact_double::operator*=(int other) {
		int next_bit = 0;
		for (int i = static_cast<int>(value.size() - 1); i > -1; --i) {
			int val = next_bit + value[i] * other;
			value[i] = val & 1;
			next_bit = val >> 1;
		}
		return *this;
	}

	range::range(): left(false), right(true) {}

	bool range::new_bit() {
		return (left.front_bit() == right.front_bit()) || !common_part.empty();
	}

	bool range::get_new_bit() {
		if (left.front_bit() == right.front_bit()) {
			common_part.push_back(left.front_bit());
			left.pop_front();
			right.pop_front();
		}
		auto res = common_part.front();
		common_part.pop_front();
		return res;
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
	}

}