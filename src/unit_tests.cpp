#include <gtest/gtest.h>

#include "circuit_file_parser.cpp"

TEST(ParserTest, SanitiyCheck) {
	CircuitFileParser Parser = CircuitFileParser();
	Parser.ParseFile("testing/test.txt");
	ASSERT_EQ(false, Parser.PerformSanityCheck());
}

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}