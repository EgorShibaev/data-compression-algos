#pragma once
#include <vector>
#include <istream>
#include <ostream>
#include <string>

namespace statistic {

	int32_t read_int32_t(std::istream &in);

	void write_int32_t(std::ostream &out, int32_t value);

	class statistic {
		int32_t data[256]{};
	public:
		statistic();

		explicit statistic(std::string &name);

		void load(std::istream &in);

		int32_t get(int);

		void unload(std::ostream &);

		int32_t sum();
	};

}