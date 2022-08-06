#pragma once

#include <string>
#include <utility>
#include <vector>

namespace bwt {

	std::vector<int> suffix_array(const std::string& str);

	std::pair<std::string, int> transform(const std::string&); // return encoded string and pos of initial string

	std::string inverse_transformation(const std::string&, int);

}
