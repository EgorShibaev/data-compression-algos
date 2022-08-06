#include "huffman/huffman_tree.hpp"
#include "huffman/huffman_coding.hpp"
#include "constants.hpp"
#include "arithmetic/arithmetic_coding.hpp"
#include "BTW/btw_coding.hpp"
#include <vector>
#include <tuple>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <ctime>

using std::string;

bool is_algo_name(const std::string& arg) {
	return arg == huffman_mode_name || arg == arithmetic_mode_name || arg == BTW_mode_name;
}

using arguments_t = std::tuple<char, string, string, string>;

arguments_t parse_args(int argc, char *argv[]) {
	if (argc != 7)
		throw std::invalid_argument("required 7 arguments");
	std::vector<std::string> args;
	for (int i = 1; i < 7; ++i)
		args.emplace_back(argv[i]);
	string file;
	string output;
	char action; // compress/decompress
	string algo;
	for (int i = 0; i < 5; ++i) {
		if (args[i] == "-f" || args[i] == "--file")
			file = args[i + 1];
		if (args[i] == "-o" || args[i] == "--output")
			output = args[i + 1];
	}
	for (int i = 0; i < 6; ++i) {
		if (args[i] == "-c" || args[i] == "-u")
			action = args[i][1];
		if (is_algo_name(args[i]))
			algo = args[i];
	}
	return {action, file, output, algo};
}

void huffman_algo(const std::string& file, const std::string& output, char mode) {
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
	}
}

void btw_algo(const std::string& file, const std::string& output, char mode) {
	if (mode == 'c') {
		std::ofstream out(output);
		std::ifstream in(file);
		btw_coding::encode(out, in);
	}
	else {
		std::ofstream out(output);
		std::ifstream in(file);
		btw_coding::decode(out, in);
	}
}

void arithmetic_algo(const std::string& file, const std::string& output, char mode) {
	if (mode == 'c') {
		std::ofstream out(output);
		std::ifstream in(file);
		out.exceptions(std::ostream::badbit | std::ostream::failbit);
		in.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		statistic::statistic stat(file);
		stat.unload(out);
		arithmetic_coding::encoder enc(stat, out);
		enc.encode(in);
	} else {
		std::ifstream in(file);
		in.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		statistic::statistic stat;
		stat.load(in);
		arithmetic_coding::decoder dec(stat, in);
		std::ofstream out(output);
		out.exceptions(std::ostream::badbit | std::ostream::failbit);
		dec.decode(out);
	}
}

int main(int argc, char *argv[]) {
	arguments_t tuple;
	try {
		tuple = parse_args(argc, argv);
	}
	catch (const std::invalid_argument& e) {
		std::cout << e.what() << "\n";
		return 1;
	}
	auto file = std::get<1>(tuple);
	auto output = std::get<2>(tuple);
	auto mode = std::get<0>(tuple);
	auto algo_name = std::get<3>(tuple);

	auto start_time = clock();

	try {
		if (algo_name == huffman_mode_name)
			huffman_algo(file, output, mode);
		else if (algo_name == arithmetic_mode_name)
			arithmetic_algo(file, output, mode);
		else if (algo_name == BTW_mode_name)
			btw_algo(file, output, mode);

		if (mode == 'c') {
			auto input_file_size = std::filesystem::file_size(file);
			auto output_file_size = std::filesystem::file_size(output);

			std::cout << "Compression is done. You can find result in " << output << " file.\n";
			std::cout << "Source file size: " << input_file_size << "byte.\n";
			std::cout << "Result file size: " << output_file_size << "byte.\n";
			std::cout << "Ratio:" << static_cast<double>(input_file_size) / output_file_size << ".\n";
		}
		else {
			std::cout << "Decompression is done. You can find result in " << output << ".\n";
		}
	}
	catch (const std::ios_base::failure& e) {
		std::cerr << e.what();
		return 1;
	}

	std::cout << "Time: " << static_cast<double>(clock() - start_time) / CLOCKS_PER_SEC << "sec\n";
}