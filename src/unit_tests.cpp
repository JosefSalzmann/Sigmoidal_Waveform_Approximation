#include <gtest/gtest.h>

#include "circuit_file_parser.cpp"

TEST(ParserTest, SanitiyCheck) {
	CircuitFileParser Parser1 = CircuitFileParser();
	Parser1.ParseFile("testing/multiple_inputs.txt");
	ASSERT_EQ(false, Parser1.PerformSanityCheck());

	CircuitFileParser Parser2 = CircuitFileParser();
	Parser2.ParseFile("testing/mutliple_gates_same_output_node.txt");
	ASSERT_EQ(false, Parser2.PerformSanityCheck());
}

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}