#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "statistic.hpp"
#include "huffman/huffman_coding.hpp"
#include "bit_manipulating.hpp"
#include "huffman/huffman_tree.hpp"
#include "constants.hpp"
#include "arithmetic/range.hpp"
#include <sstream>
#include "arithmetic/arithmetic_coding.hpp"
#include "BTW/btw.hpp"
#include "BTW/rle.hpp"
#include "BTW/btw_coding.hpp"

TEST_CASE("bit_reader tests") {
	std::stringstream stream;
	bit_manipulating::bit_reader reader(stream);

	SUBCASE("one byte 255") {
		stream.put((char) 255);
		for (int i = 0; i < 8; ++i) {
			CHECK(reader.read_bit());
		}
	}

	SUBCASE("one byte 0") {
		stream.put((char) 0);
		for (int i = 0; i < 8; ++i) {
			CHECK(!reader.read_bit());
		}
	}

	SUBCASE("one two bytes: 255 and 0") {
		stream.put((char) 255);
		stream.put((char) 0);
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
	CHECK((unsigned char) output_stream.get() == 0b01100101);
	CHECK((unsigned char) output_stream.get() == 0b11100110);
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
	input_stream.put(static_cast<char>(0b10001111));
	input_stream.put(static_cast<char>(0b00000001));
	dec.decode(output_stream);
	std::string res;
	output_stream >> res;
	CHECK(res == "hello");
}

TEST_CASE("huffman tree test") {

	SUBCASE("test huffman tree node") {
		auto *left_node = new huffman_tree::huffman_tree_node('a', 3);
		auto *right_node = new huffman_tree::huffman_tree_node('b', 2);
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
	CHECK(!r.new_byte());
	r.change_according_to_char(14, 15, 9);// [0.000001110 0.000001111]
	CHECK(r.new_byte());
	CHECK(r.get_new_byte() == 0b000000000);
	r.pop_new_byte();
	CHECK(r.new_byte());
	CHECK(r.get_new_byte() == 0b00000111);
	r.pop_new_byte();
	CHECK(!r.new_byte());

	range::range r3;
	r3.change_according_to_char(0, 2, 2); // [0 1] -> [0.0 0.1]
	r3.change_according_to_char(2, 4, 2); // [0.0 0.1] -> [0.01 0.10]
	r3.change_according_to_char(0, 2, 2); // [0.01 0.10] -> [0.010 0.011]
	std::stringstream stream;
	r3.show(stream);
	std::string left, right;
	stream >> left >> right;
	while (left[left.length() - 1] == '0')
		left.pop_back();
	while (right[right.length() - 1] == '0')
		right.pop_back();
	CHECK(left == "0000000001");
	CHECK(right == "00000000011");
}

TEST_CASE("adj stat test") {
	statistic::statistic stat;
	stat.set(0, 1);
	stat.set(1, 1);
	auto adj = arithmetic_coding::adjust_stat(stat);
	CHECK(adj.get(0) == 1);
	CHECK(adj.get(1) == 1);
	CHECK(adj.get(2) == 0);
	stat.set(2, 1);
	adj = arithmetic_coding::adjust_stat(stat);
	CHECK(adj.get(0) == 2);
	CHECK(adj.get(1) == 1);
	CHECK(adj.get(2) == 1);
	stat.set(1, 3);
	adj = arithmetic_coding::adjust_stat(stat); // 1 3 1 -> 1 4 1 -> 2 5 1
	CHECK(adj.get(0) == 2);
	CHECK(adj.get(1) == 5);
	CHECK(adj.get(2) == 1);

}

TEST_CASE("arithmetic encoder test") {
	SUBCASE("AAA") {
		std::stringstream output_stream;
		std::stringstream input_stream;
		statistic::statistic stat;
		stat.set(static_cast<unsigned char>('A'), 3); // adj: A-4
		{
			arithmetic_coding::encoder encoder(stat, output_stream);
			input_stream << "AAA";
			encoder.encode(input_stream);
		} // [0 1] -> [0 1] -> [0 1] -> [0 1] -> 0.1
		CHECK(static_cast<unsigned char>(output_stream.get()) == 0b00000000);
		CHECK(static_cast<unsigned char>(output_stream.get()) == 0b10000000);
	}

	SUBCASE("abc") {
		std::stringstream output_stream;
		std::stringstream input_stream;
		statistic::statistic stat;
		stat.set(static_cast<unsigned char>('a'), 1);
		stat.set(static_cast<unsigned char>('b'), 1);
		stat.set(static_cast<unsigned char>('c'), 1); // adj: a-2 b-1 c-1
		{
			arithmetic_coding::encoder encoder(stat, output_stream);
			input_stream << "abc";
			encoder.encode(input_stream);
		} // [0 1] -> [0.0 0.1] -> [0.010 0.011] -> [0.01011  0.01100] -> 0.010111
		CHECK(static_cast<unsigned char>(output_stream.get()) == 0b00000000);
		CHECK(static_cast<unsigned char>(output_stream.get()) == 0b01011100);
	}

	SUBCASE("ab") {
		std::stringstream output_stream;
		std::stringstream input_stream;
		statistic::statistic stat;
		stat.set(static_cast<unsigned char>('a'), 1);
		stat.set(static_cast<unsigned char>('b'), 1);
		{
			arithmetic_coding::encoder encoder(stat, output_stream);
			input_stream << "ab";
			encoder.encode(input_stream);
		} // [0 1] -> [0.0; 0.1] -> [0.01 0.10] -> [0.011]
		CHECK(static_cast<unsigned char>(output_stream.get()) == 0b00000000);
		CHECK(static_cast<unsigned char>(output_stream.get()) == 0b01100000);
	}

	SUBCASE("abb") {
		std::stringstream output_stream;
		std::stringstream input_stream;
		statistic::statistic stat;
		stat.set(static_cast<unsigned char>('a'), 1);
		stat.set(static_cast<unsigned char>('b'), 2); // adj a-2 b-2
		{
			arithmetic_coding::encoder encoder(stat, output_stream);
			input_stream << "abb";
			encoder.encode(input_stream);
		} // [0 1] -> [0.0 0.1] -> [0.01 0.10] -> [0.011 0.100] -> 0.0111
		CHECK(static_cast<unsigned char>(output_stream.get()) == 0b00000000);
		CHECK(static_cast<unsigned char>(output_stream.get()) == 0b01110000);
	}
}

TEST_CASE("check arithmetic decode") {
	SUBCASE("AAA") {
		std::stringstream output_stream;
		std::stringstream input_stream;
		statistic::statistic stat;
		stat.set(static_cast<unsigned char>('A'), 3); // adj: A-4
		{
			arithmetic_coding::decoder decoder(stat, input_stream);
			input_stream.put(static_cast<char>(0b00000000));
			input_stream.put(static_cast<char>(0b10000000));
			decoder.decode(output_stream);
		} // [0 1] -> [0 1] -> [0 1] -> [0 1] -> 0.1
		std::string res;
		output_stream >> res;
		CHECK(res == "AAA");
	}

	SUBCASE("abc") {
		std::stringstream output_stream;
		std::stringstream input_stream;
		statistic::statistic stat;
		stat.set(static_cast<unsigned char>('a'), 1);
		stat.set(static_cast<unsigned char>('b'), 1);
		stat.set(static_cast<unsigned char>('c'), 1); // adj: a-2 b-1 c-1
		{
			arithmetic_coding::decoder decoder(stat, input_stream);
			input_stream.put(static_cast<char>(0b00000000));
			input_stream.put(static_cast<char>(0b01011100));
			decoder.decode(output_stream);
		}
		std::string res;
		output_stream >> res;
		CHECK(res == "abc");
	}

	SUBCASE("ab") {
		std::stringstream output_stream;
		std::stringstream input_stream;
		statistic::statistic stat;
		stat.set(static_cast<unsigned char>('a'), 1);
		stat.set(static_cast<unsigned char>('b'), 1);
		{
			arithmetic_coding::decoder decoder(stat, input_stream);
			input_stream.put(static_cast<char>(0b00000000));
			input_stream.put(static_cast<char>(0b01100000));
			decoder.decode(output_stream);
		}
		std::string res;
		output_stream >> res;
		CHECK(res == "ab");
	}

	SUBCASE("abb") {
		std::stringstream output_stream;
		std::stringstream input_stream;
		statistic::statistic stat;
		stat.set(static_cast<unsigned char>('a'), 1);
		stat.set(static_cast<unsigned char>('b'), 2);
		{
			arithmetic_coding::decoder decoder(stat, input_stream);
			input_stream.put(static_cast<char>(0b00000000));
			input_stream.put(static_cast<char>(0b01110000));
			decoder.decode(output_stream);
		}
		std::string res;
		output_stream >> res;
		CHECK(res == "abb");

	}
}

TEST_CASE("arithmetic encoder + decoder test") {
	std::string text = "abcdef";
	std::stringstream stream1;
	std::stringstream stream2;
	std::stringstream stream3;
	stream1 << text;
	statistic::statistic stat;
	stat.set(static_cast<unsigned char>('a'), 1);
	stat.set(static_cast<unsigned char>('b'), 1);
	stat.set(static_cast<unsigned char>('c'), 1);
	stat.set(static_cast<unsigned char>('d'), 1);
	stat.set(static_cast<unsigned char>('e'), 1);
	stat.set(static_cast<unsigned char>('f'), 1);
	{
		arithmetic_coding::encoder enc(stat, stream2);
		enc.encode(stream1);
	}
	{
		arithmetic_coding::decoder dec(stat, stream2);
		dec.decode(stream3);
	}
	std::string res;
	stream3 >> res;
	CHECK(text == res);
}

TEST_CASE("suffix array test") {
	CHECK(btw::suffix_array("abcd") == std::vector{0, 1, 2, 3});
	CHECK(btw::suffix_array("aaba") == std::vector{3, 0, 1, 2});
	CHECK(btw::suffix_array("abaabbaba") == std::vector{8, 2, 6, 0, 3, 7, 1, 5, 4});
}

TEST_CASE("btw transformation test") {
	CHECK(btw::transform("abcd") == std::make_pair(std::string("dabc"), 0));
	CHECK(btw::transform("aaba") == std::make_pair(std::string("baaa"), 1));
	CHECK(btw::transform("^BANANA|") == std::make_pair(std::string("BNN^AA|A"), 6));
}

TEST_CASE("btw inverse transformation test") {
	CHECK(btw::inverse_transformation("dabc", 0) == "abcd");
	CHECK(btw::inverse_transformation("baaa", 1) == "aaba");
	CHECK(btw::inverse_transformation("BNN^AA|A", 6) == "^BANANA|");
}

TEST_CASE("btw full test") {
	SUBCASE("1") {
		std::string text = "kadjflkdshkdjghdsfcvbdfbv bsfdg  r49i324 rq r";
		auto tr = btw::transform(text);
		CHECK(btw::inverse_transformation(tr.first, tr.second) == text);
	}

	SUBCASE("2") {
		std::string text = "jdfhadsfbanmfbadsmfndasfdasfdasfdakjdsafhdasjkfhhhhhhfmnsdbsssss";
		auto tr = btw::transform(text);
		CHECK(btw::inverse_transformation(tr.first, tr.second) == text);
	}

	SUBCASE("3") {
		std::string text = "1. The Raven \n"
		                   "by Edgar Allen Poe\n"
		                   "Deep into that darkness peering,\n"
		                   "Long I stood there, wondering, fearing,\n"
		                   "Doubting, dreaming dreams no mortals\n"
		                   "Ever dared to dream before;\n"
		                   "But the silence was unbroken,\n"
		                   "And the stillness gave no token,\n"
		                   "And the only word there spoken\n"
		                   "Was the whispered word, \"Lenore!\"\n"
		                   "This I whispered, and an echo\n"
		                   "Murmured back the word, \"Lenore!\"\n"
		                   "Merely this, and nothing more.\n"
		                   "2. The New Colossus\n"
		                   "by Emma Lazarus\n"
		                   "Not like the brazen giant of Greek fame,\n"
		                   "With conquering limbs astride from land to land;\n"
		                   "Here at our sea-washed, sunset gates shall stand\n"
		                   "A mighty woman with a torch, whose flame\n"
		                   "Is the imprisoned lightning, and her name\n"
		                   "Mother of Exiles. From her beacon-hand\n"
		                   "Glows world-wide welcome; her mild eyes command\n"
		                   "The air-bridged harbor that twin cities frame.\n"
		                   "“Keep, ancient lands, your storied pomp!” cries she\n"
		                   "  So long lives this, and this gives life to thee.";
		auto tr = btw::transform(text);
		CHECK(btw::inverse_transformation(tr.first, tr.second) == text);
	}
}

TEST_CASE("rle transformation test") {
	CHECK(rle::transform("aaaaaabcde") == std::string(1, 6) + "a" + std::string(1, -4) + "bcde");
	CHECK(rle::transform("aaaaaaaaa") == std::string(1, 9) + "a");
	CHECK(rle::transform("abcdeeeeee") == std::string(1, -4) + "abcd" + std::string(1, 6) + "e");
	CHECK(rle::transform("abcddddcba") ==
	      std::string(1, -3) + "abc" + std::string(1, 4) + "d" + std::string(1, -3) + "cba");
}

TEST_CASE("rle inverse transformation test") {
	rle::inv_transformator tr(6);
	tr.add_char(6);
	CHECK(!tr.is_ready());
	tr.add_char('a');
	CHECK(tr.is_ready());
	CHECK(tr.get_string() == "aaaaaa");
	tr.add_char(-4);
	tr.add_char('b');
	tr.add_char('c');
	tr.add_char('d');
	tr.add_char('e');
	CHECK(!tr.is_ready());
	CHECK(tr.get_string() == "bcde");

}

TEST_CASE("btw+rle coding test") {
	std::stringstream stream1;
	std::stringstream stream2;

	stream1 << "aaba";
	btw_coding::encode(stream2, stream1); // aaba -> baaa -> -1 b 3 a
	CHECK(statistic::read_int32_t(stream2) == 1);
	CHECK(static_cast<char>(stream2.get()) == -1);
	CHECK(static_cast<char>(stream2.get()) == 'b');
	CHECK(static_cast<char>(stream2.get()) == 3);
	CHECK(static_cast<char>(stream2.get()) == 'a');
}

TEST_CASE("btw+rle decoding test") {
	std::stringstream stream1;
	std::stringstream stream2;

	statistic::write_int32_t(stream1, 1);
	stream1.put(static_cast<char>(-2));
	stream1.put(static_cast<char>('b'));
	stream1.put(static_cast<char>('a'));
	stream1.put(static_cast<char>(2));
	stream1.put(static_cast<char>('a'));
	btw_coding::decode(stream2, stream1);
	std::string source;
	stream2 >> source;
	CHECK(source == "aaba");
}

TEST_CASE("bwt+rle encoding+decoding test") {
	SUBCASE("small") {
		std::string text("jdfhadsfbanmfbadsmfndasfdasfdasfdakjdsafhdasjkfhhhhhhfmnsdbsssss");
		std::stringstream stream1;
		std::stringstream stream2;
		std::stringstream stream3;
		stream1 << text;
		btw_coding::encode(stream2, stream1);
		btw_coding::decode(stream3, stream2);
		std::string res;
		stream3 >> res;
		CHECK(text == res);
	}

	SUBCASE("big") {
		std::string text; // a..ab..b
		int len = 1000000;
		for (int i = 0; i < len; ++i)
			text.push_back(i < len / 2 ? 'a' : 'b');
		std::stringstream stream1;
		std::stringstream stream2;
		std::stringstream stream3;
		stream1 << text;
		btw_coding::encode(stream2, stream1);
		btw_coding::decode(stream3, stream2);
		std::string res;
		stream3 >> res;
		CHECK(text == res);
	}

	SUBCASE("other big") {

		std::string text;
		int len = 1000000;
		for (int i = 0; i < len; ++i)
			text.push_back(static_cast<char>('a' + (i % 26)));
		std::stringstream stream1;
		std::stringstream stream2;
		std::stringstream stream3;
		stream1 << text;
		btw_coding::encode(stream2, stream1);
		btw_coding::decode(stream3, stream2);
		std::string res;
		stream3 >> res;
		CHECK(text == res);
	}
}