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
  std::cout << "Do you already have the file with grammar? (Y/N)\n";
  char file_already_exists;
  std::cin >> file_already_exists;
  if (file_already_exists == 'Y') {
    std::string file_name;
    std::cout << "Enter grammar file name\n";
    std::cin >> file_name;
    bool successful_creation = true;
    Grammar grammar(file_name, successful_creation);
    if (successful_creation) {
      std::string word;
      std::cout << "Enter word\n";
      std::cin >> word;
      PrintWordExistenceResult(grammar, word);
    } else {
      std::cout << "Incorrect file\n";
    }
  } else {
    std::string curr_string;
    std::fstream fs;
    fs.open("grammar_file.txt", std::fstream::in | std::fstream::out);
    std::cout << "Enter all not terminal symbols from grammar\nFor example ABC\n";
    std::cin >> curr_string;
    fs << curr_string << '\n';
    std::cout << "Enter all terminal symbols from grammar\nFor example a+(\n";
    std::cin >> curr_string;
    fs << curr_string << '\n';
    std::cout << "Enter grammar separator\nFor example >\n";
    std::cin >> curr_string;
    fs << curr_string << '\n';
    std::cout << "Enter rules (one line - one rule)\nFor example A>a\n";
    std::cout << "First line after the rules must be 'finish'\n";
    std::cin >> curr_string;
    while (!(curr_string == "finish")) {
      fs << curr_string << '\n';
      std::cin >> curr_string;
    }
    bool successful_creation = true;
    Grammar grammar(std::string("../build/grammar_file.txt"), successful_creation);
    if (successful_creation) {
      std::string word;
      std::cout << "Enter word\n";
      std::cin >> word;
      PrintWordExistenceResult(grammar, word);
    } else {
      std::cout << "Incorrect data\n";
    }
    fs.close();
  }
  return 0;
}