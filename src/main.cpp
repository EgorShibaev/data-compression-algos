#include "../include/huffman_tree.hpp"
#include "../include/huffman_coding.hpp"
#include "../include/constants.hpp"
#include <vector>
#include <tuple>
#include <fstream>
#include <iostream>
#include <stdexcept>

std::tuple<char, std::string, std::string> parse_args(int argc, char *argv[]) {
	if (argc != 6)
		throw std::invalid_argument("required 6 arguments");
	std::vector<std::string> args;
	for (int i = 1; i < 6; ++i)
		args.emplace_back(argv[i]);
	std::string file;
	std::string output;
	char mode;
	for (int i = 0; i < 4; ++i)
		if (args[i] == "-f" || args[i] == "--file")
			file = args[i + 1];
	for (int i = 0; i < 4; ++i)
		if (args[i] == "-o" || args[i] == "--output")
			output = args[i + 1];
	for (int i = 0; i < 4; ++i)
		if (args[i] == "-c" || args[i] == "-u")
			mode = args[i][1];
	return {mode, file, output};
}

const int table_size = int32_t_size * bytes_count;

int main(int argc, char *argv[]) {
	std::tuple<char, std::string, std::string> tuple;
	try {
		tuple = parse_args(argc, argv);
	}
	catch (const std::invalid_argument& e) {
		std::cout << e.what() << "\n";
		return 1;
	}
	std::string file = std::get<1>(tuple);
	std::string output = std::get<2>(tuple);
	char mode = std::get<0>(tuple);

	int input_file_size = -1;
	int output_file_size = -1;

	try {
		if (mode == 'c') {
			std::ofstream out(output);
			out.exceptions(std::ostream::badbit | std::ostream::failbit);
			statistic::statistic stat(file);
			stat.unload(out);
			huffman_tree::huffman_tree tree(stat);
			huffman_coding::encoder enc(tree.codes(), stat.sum(), out);
			std::ifstream in(file);
			in.exceptions(std::ifstream::badbit | std::ifstream::failbit);
			enc.encode(in);

			input_file_size = stat.sum();
		} else {
			std::ifstream in(file);
			in.exceptions(std::ifstream::badbit | std::ifstream::failbit);
			statistic::statistic stat;
			stat.load(in);
			huffman_tree::huffman_tree tree(stat);
			huffman_coding::decoder dec(std::move(tree), stat.sum(), in);
			std::ofstream out(output);
			out.exceptions(std::ostream::badbit | std::ostream::failbit);
			dec.decode(out);

			input_file_size = bit_manipulating::bit_reader::read_byte_count;
			output_file_size = stat.sum();
		}
	}
	catch (const std::ios_base::failure& e) {
		std::cerr << e.what();
		return 1;
	}
	if (output_file_size == -1)
		output_file_size = bit_manipulating::bit_writer::written_byte_count;

	std::cout << input_file_size << "\n";
	std::cout << output_file_size << "\n";
	std::cout << table_size << "\n";
}