#include <gtest/gtest.h>

#include "../circuit_file_parser.cpp"

TEST(ParserTest, SanitiyCheck) {
	CircuitFileParser Parser1 = CircuitFileParser();
	bool fails = false;
	try {
		Parser1.ParseFile("testing/test_data/multiple_inputs.txt");
		Parser1.PerformSanityCheck();
	} catch (const std::exception& e) {
		fails = true;
	}
	ASSERT_EQ(true, fails);

	Parser1 = CircuitFileParser();
	fails = false;
	try {
		Parser1.ParseFile("testing/test_data/mutliple_gates_same_output_node.txt");
		Parser1.PerformSanityCheck();
	} catch (const std::exception& e) {
		fails = true;
	}
	ASSERT_EQ(true, fails);

	Parser1 = CircuitFileParser();
	fails = false;
	try {
		Parser1.ParseFile("testing/test_data/input_and_gate_same_output_node.txt");
		Parser1.PerformSanityCheck();
	} catch (const std::exception& e) {
		fails = true;
	}
	ASSERT_EQ(true, fails);
}