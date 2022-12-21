#include <iostream>
#include "include/grammar.h"
#include <fstream>

int main() {
  std::string file;
  std::cin >> file;
  std::fstream fs;
  fs.open(file);
  Grammar grammar(fs);
  auto chomsky = ChomskyNormalForm::MakeChomskyNormalForm(grammar);
  std::string word;
  int amount = 0;
  std::cin >> amount;
  while(0 < amount--) {
    std::cin >> word;
    if (chomsky.CYK(word)) {
      std::cout << "The word " << word << " belongs to the grammar\n";
    } else {
      std::cout << "The word " << word << " doesn't belong to the grammar\n";
    }
  }
  fs.close();
  return 0;
}