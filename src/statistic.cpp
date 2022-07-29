#include "statistic.hpp"
#include "constants.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace statistic {

	int32_t read_int32_t(std::istream &in) {
		int32_t value = 0;
		for (int i = 0; i < int32_t_size; ++i) {
			char curr;
			in.get(curr);
			value += (unsigned char) curr * (1 << (byte_size * i));
		}
		return value;
	}

	void write_int32_t(std::ostream &out, int32_t value) {
		for (int i = 0; i < int32_t_size; ++i) {
			out.put((char) (value & ((1 << byte_size) - 1)));
			value >>= 8;
		}
	}

	statistic::statistic() = default;

	statistic::statistic(const std::string &name) {
		std::ifstream in(name);
		in.exceptions(std::ifstream::badbit);
		char ch;
		in.get(ch);
		while (in) {
			data[(unsigned char) ch]++;
			ch = in.get();
		}
	}

	void statistic::load(std::istream &in) {
		for (int32_t &i: data) {
			i = read_int32_t(in);
		}
	}

	int32_t statistic::get(int i) {
		return data[i];
	}

	void statistic::set(int i, int value) {
		data[i] = value;
	}

	void statistic::unload(std::ostream &out) {
		for (int32_t i: data) {
			write_int32_t(out, i);
		}
	}

	int32_t statistic::sum() {
		int res = 0;
		for (int i = 0; i < bytes_count; ++i)
			res += data[i];
		return res;
	}

}