#include <gtest/gtest.h>
#include "fstream"
#include "../include/grammar.h"
#include "../include/earley.h"

GTEST_API_ int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

bool IsWordInGrammar(const std::string& path, const std::string word) {
  std::fstream fs;
  fs.open(path);
  bool success = true;
  Grammar grammar(fs, success);
  Earley ealey_alg;
  return ealey_alg.CheckWord(grammar, word);
}

TEST(Correct_bracket_sequences_grammar, Correct_bracket_sequences_grammar_Test) {
  std::string word("()(((())))()(())");
  std::string path("../tests/correct_bracket_sequences_grammar.txt");
  ASSERT_TRUE(IsWordInGrammar(path, word));
  word = "()()";
  ASSERT_TRUE(IsWordInGrammar(path, word));
  word = "(((())()";
  ASSERT_FALSE(IsWordInGrammar(path, word));
}

TEST(Sum_and_multiplication, Sum_and_multiplication_Test) {
  std::string word("(x*x+x)");
  std::string path("../tests/sum_multi_grammar.txt");
  ASSERT_TRUE(IsWordInGrammar(path, word));
  word = "(x+x)x*";
  ASSERT_FALSE(IsWordInGrammar(path, word));
}