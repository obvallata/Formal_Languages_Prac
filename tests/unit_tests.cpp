#include <gtest/gtest.h>
#include "../include/grammar.h"

GTEST_API_ int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

TEST(Correct_bracket_sequences_grammar, Correct_bracket_sequences_grammar_Test) {
  std::string path("../tests/correct_bracket_sequences_grammar.txt");
  std::fstream fs;
  fs.open(path);
  Grammar grammar(fs);
  auto chomsky = ChomskyNormalForm::MakeChomskyNormalForm(grammar);
  std::string word("()(((())))()(())");
  ASSERT_TRUE(chomsky.CYK(word));
  word = "()()";
  ASSERT_TRUE(chomsky.CYK(word));
  word = "(((())()";
  ASSERT_FALSE(chomsky.CYK(word));
}

TEST(Sum_and_multiplication, Sum_and_multiplication_Test) {
  std::string path("../tests/sum_multi_grammar.txt");
  std::fstream fs;
  fs.open(path, std::fstream::in);
  Grammar grammar(fs);
  auto chomsky = ChomskyNormalForm::MakeChomskyNormalForm(grammar);
  std::string word("(x*x+x)");
  ASSERT_TRUE(chomsky.CYK(word));
  word = "(x+x)x*";
  ASSERT_FALSE(chomsky.CYK(word));
}

TEST(Abc_grammar, Abc_grammar_At_Test) {
  std::string path("../tests/abc_grammar.txt");
  std::fstream fs;
  fs.open(path, std::fstream::in);
  Grammar grammar(fs);
  auto chomsky = ChomskyNormalForm::MakeChomskyNormalForm(grammar);
  std::string word("ababc");
  ASSERT_TRUE(chomsky.CYK(word));
  word = "ababcc";
  ASSERT_FALSE(chomsky.CYK(word));
}
