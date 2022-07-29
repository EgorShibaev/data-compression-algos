#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "statistic.hpp"
#include "huffman/huffman_coding.hpp"
#include "bit_manipulating.hpp"
#include "huffman/huffman_tree.hpp"
#include "constants.hpp"
#include "arithmetic/range.hpp"
#include <sstream>

TEST_CASE("bit_reader tests") {
	std::stringstream stream;
	bit_manipulating::bit_reader reader(stream);

	SUBCASE("one byte 255") {
		stream.put((char)255);
		for (int i = 0; i < 8; ++i) {
			CHECK(reader.read_bit());
		}
	}

	SUBCASE("one byte 0") {
		stream.put((char)0);
		for (int i = 0; i < 8; ++i) {
			CHECK(!reader.read_bit());
		}
	}

	SUBCASE("one two bytes: 255 and 0") {
		stream.put((char)255);
		stream.put((char)0);
		for (int i = 0; i < 8; ++i) {
			CHECK(reader.read_bit());
		}
		for (int i = 0; i < 8; ++i) {
			CHECK(!reader.read_bit());
		}
	}
}

TEST_CASE("bit_writer test") {
	std::stringstream stream;
	bit_manipulating::bit_writer writer(stream);

	SUBCASE("one byte 255") {
		for (int i = 0; i < 8; ++i) {
			writer.write_bit(true);
		}
		CHECK(stream.get() == 255);
	}

	SUBCASE("one byte 255") {
		for (int i = 0; i < 8; ++i) {
			writer.write_bit(false);
		}
		CHECK(stream.get() == 0);
	}
}

TEST_CASE("statistic class test") {
	statistic::statistic stat;

	std::stringstream output_stream;
	std::stringstream input_stream;

	std::vector<int> freq(bytes_count);
	for (int i = 0; i < bytes_count; ++i) {
		freq[i] = i;
		statistic::write_int32_t(output_stream, freq[i]);
	}

	stat.load(output_stream);
	for (int i = 0; i < bytes_count; ++i) {
		CHECK(stat.get(i) == freq[i]);
	}
	CHECK(stat.sum() == (bytes_count * (bytes_count - 1)) / 2);

	stat.unload(input_stream);
	for (int i = 0; i < bytes_count; ++i) {
		CHECK(freq[i] == statistic::read_int32_t(input_stream));
	}
}

TEST_CASE("encoder test") {
	std::vector<std::string> codes(256);
	codes['h'] = "1";
	codes['e'] = "010";
	codes['l'] = "0110";
	codes['o'] = "0111";
	std::stringstream output_stream;
	std::stringstream input_stream;
	huffman_coding::encoder enc(codes, 5, output_stream);

	input_stream << "hello";
	enc.encode(input_stream);
	CHECK((unsigned char)output_stream.get() == 0b01100101);
	CHECK((unsigned char)output_stream.get() == 0b11100110);
}

TEST_CASE("decoder and huffman tree test") {
	statistic::statistic stat;
	std::stringstream stat_buffer;
	for (int i = 0; i < bytes_count; ++i) {
		if (i == 'h' || i == 'e' || i == 'o')
			statistic::write_int32_t(stat_buffer, 1);
		else if (i == 'l')
			statistic::write_int32_t(stat_buffer, 2);
		else
			statistic::write_int32_t(stat_buffer, 0);
	}
	stat.load(stat_buffer);
	huffman_tree::huffman_tree tree(stat);
	auto codes = tree.codes();
	for (int i = 0; i < bytes_count; ++i) {
		if (i == 'h')
			CHECK(codes[i] == "111");
		else if (i == 'e')
			CHECK(codes[i] == "10");
		else if (i == 'l')
			CHECK(codes[i] == "0");
		else if (i == 'o')
			CHECK(codes[i] == "110");
		else
			CHECK(codes[i] == "");
	}

	std::stringstream output_stream;
	std::stringstream input_stream;
	huffman_coding::decoder dec(std::move(tree), 5, input_stream);
	input_stream.put(0b10001111);
	input_stream.put(0b00000001);
	dec.decode(output_stream);
	std::string res;
	output_stream >> res;
	CHECK(res == "hello");
}

TEST_CASE("huffman tree test") {

	SUBCASE("test huffman tree node") {
		auto* left_node = new huffman_tree::huffman_tree_node('a', 3);
		auto* right_node = new huffman_tree::huffman_tree_node('b', 2);
		huffman_tree::huffman_tree_node main_node(left_node, right_node);

		CHECK(left_node->is_leaf());
		CHECK(left_node->str() == "a");
		CHECK(left_node->count() == 3);

		CHECK(right_node->is_leaf());
		CHECK(right_node->str() == "b");
		CHECK(right_node->count() == 2);

		CHECK(main_node.left() == left_node);
		CHECK(main_node.right() == right_node);
		CHECK(main_node.str() == "ab");
		CHECK(main_node.count() == 5);
	}

	SUBCASE("three vertex") {
		statistic::statistic stat;

		std::stringstream stream;

		std::vector<int> freq(bytes_count);
		for (int i = 0; i < bytes_count; ++i) {
			if (i >= 'a' && i <= 'c')
				freq[i] = pow(i - 'a', 3) + 1;
			statistic::write_int32_t(stream, freq[i]);
		}
		stat.load(stream);
		huffman_tree::huffman_tree tree(stat);

		auto curr = tree.root();

		CHECK(curr->str() == "abc");
		CHECK(!curr->is_leaf());
		CHECK(curr->count() == 12);

		REQUIRE(curr->left() != nullptr);
		REQUIRE(curr->right() != nullptr);

		CHECK(curr->right()->is_leaf());
		CHECK(curr->right()->count() == 9);
		CHECK(curr->right()->str() == "c");
		CHECK(curr->right()->left() == nullptr);
		CHECK(curr->right()->right() == nullptr);

		curr = curr->left();

		CHECK(curr->str() == "ab");
		CHECK(!curr->is_leaf());
		CHECK(curr->count() == 3);


		REQUIRE(curr->left() != nullptr);
		REQUIRE(curr->right() != nullptr);

		CHECK(curr->right()->is_leaf());
		CHECK(curr->left()->is_leaf());
		CHECK(curr->right()->count() == 2);
		CHECK(curr->left()->count() == 1);
		CHECK(curr->right()->str() == "b");
		CHECK(curr->left()->str() == "a");

		auto codes = tree.codes();
		CHECK(codes['a'] == "00");
		CHECK(codes['b'] == "01");
		CHECK(codes['c'] == "1");
	}

	SUBCASE("one vertex") {
		statistic::statistic stat;

		std::stringstream stream;

		for (int i = 0; i < bytes_count; ++i) {
			if (i == 'a')
				statistic::write_int32_t(stream, 4);
			else
				statistic::write_int32_t(stream, 0);
		}
		stat.load(stream);
		huffman_tree::huffman_tree tree(stat);

		CHECK(tree.root()->is_leaf());
		CHECK(tree.root()->count() == 4);
		CHECK(tree.root()->str() == "a");

		auto codes = tree.codes();
		CHECK(codes['a'] == "");
	}

	SUBCASE("zero vertex") {
		statistic::statistic stat;
		std::stringstream stream;
		for (int i = 0; i < bytes_count; ++i)
			statistic::write_int32_t(stream, 0);
		stat.load(stream);
		huffman_tree::huffman_tree tree(stat);
		CHECK(tree.root() == nullptr);
		auto codes = tree.codes();
		for (int i = 0; i < bytes_count; ++i)
			CHECK(codes[i] == "");
	}

	SUBCASE("test move constructor") {
		statistic::statistic stat;
		std::stringstream stream;
		for (int i = 0; i < bytes_count; ++i)
			statistic::write_int32_t(stream, 42);
		stat.load(stream);
		huffman_tree::huffman_tree tree(stat);
		huffman_tree::huffman_tree other_tree = std::move(tree);

		CHECK(tree.root() == nullptr);
		REQUIRE(other_tree.root() != nullptr);
		CHECK(other_tree.root()->count() == 42 * bytes_count);
		CHECK(other_tree.root()->str().size() == bytes_count);

	}
}

TEST_CASE("range test") {
	range::range r;
	CHECK(!r.new_bit());
	r.change_according_to_char(1, 3, 2);
	CHECK(r.new_bit());
	CHECK(!r.get_new_bit());
	CHECK(!r.new_bit());
	r.change_according_to_char(0, 1, 1);
	CHECK(!r.new_bit());
	r.change_according_to_char(1, 2, 1);
	CHECK(!r.new_bit());
	r.change_according_to_char(0, 1, 1);
	CHECK(r.new_bit());
	CHECK(!r.get_new_bit());
	CHECK(r.new_bit());
	CHECK(r.get_new_bit());
	CHECK(r.new_bit());
	CHECK(r.get_new_bit());
	CHECK(!r.new_bit());
}