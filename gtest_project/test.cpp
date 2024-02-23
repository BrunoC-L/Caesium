#include "pch.h"
#include "test_parse.hpp"
#include "test_transpile_files.hpp"

TEST(TestCaseName, TestName) {
	EXPECT_TRUE(testParse());
	EXPECT_TRUE(test_transpile_all_folders(std::filesystem::directory_iterator{ "C:\\Users\\Bruno\\Desktop\\prog\\src\\c++\\Caesium\\Tests\\Tests" }));
}
