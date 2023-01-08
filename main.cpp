#include <iostream>
#include "include/grammar.h"
#include "include/earley.h"
#include <string>
#include <fstream>

void PrintWordExistenceResult(const Grammar& grammar, const std::string& word) {
  Earley earley_alg;
  if (earley_alg.CheckWord(grammar, word)) {
    std::cout << "The word belongs to the grammar\n";
  } else {
    std::cout << "The word doesn't belong to the grammar\n";
  }
}

int main() {
  std::string file_name;
  std::cout << "Enter grammar file name\n";
  std::cin >> file_name;
  std::fstream fs;
  fs.open(file_name, std::fstream::in);
  bool successful_creation = true;
  Grammar grammar(fs, successful_creation);
  if (successful_creation) {
    std::string word;
    std::cout << "Enter word (if you want to check an empty word enter $)\n";
    std::cin >> word;
    if (word == "$") {
      word = "";
    }
    PrintWordExistenceResult(grammar, word);
  } else {
    std::cout << "Incorrect file\n";
  }
  return 0;
}