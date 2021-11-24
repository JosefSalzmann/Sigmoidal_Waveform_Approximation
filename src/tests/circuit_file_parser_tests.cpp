#include <gtest/gtest.h>

#include "../circuit_file_parser.cpp"

TEST(ParserTest, SanitiyCheck) {
	CircuitFileParser Parser1 = CircuitFileParser();
	Parser1.ParseFile("testing/test_data/multiple_inputs.txt");
	ASSERT_EQ(false, Parser1.PerformSanityCheck());

	Parser1 = CircuitFileParser();
	Parser1.ParseFile("testing/test_data/mutliple_gates_same_output_node.txt");
	ASSERT_EQ(false, Parser1.PerformSanityCheck());

	Parser1 = CircuitFileParser();
	Parser1.ParseFile("testing/test_data/input_and_gate_same_output_node.txt");
	ASSERT_EQ(false, Parser1.PerformSanityCheck());
}