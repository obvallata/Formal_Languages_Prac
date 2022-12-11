#include <algorithm>
#include "fstream"
#include "../include/grammar.h"

Grammar::Grammar(std::basic_fstream<char>& fs, bool& successful_creation) {
  fs.seekg(0);
  if (!fs.is_open()) {
    successful_creation = false;
    return;
  }
  std::string new_rule;
  std::getline(fs, new_rule);
  for (auto& letter : new_rule) {
    AddSupportive(letter);
  }
  std::getline(fs, new_rule);
  for (auto& letter : new_rule) {
    AddTerminal(letter);
  }
  std::getline(fs, new_rule);
  SetInRuleSeparator(new_rule[0]);
  while (std::getline(fs, new_rule) && successful_creation) {
    successful_creation = AddRule(new_rule);
  }
  fs.close();
}

bool Grammar::AddRule(const std::string& rule) {
  auto separator = std::find(rule.begin(), rule.end(), in_rule_separator_);
  if (separator == rule.end()) {
    return false;
  }
  rules_[rule[0]].emplace_back(rule.substr(2, rule.size() - 2));
  return true;
}

void Grammar::AddTerminal(const char& letter) {
  terminals_.emplace(letter);
}

void Grammar::AddSupportive(const char& letter) {
  supportive_.emplace(letter);
}

void Grammar::SetInRuleSeparator(const char& separator) {
  in_rule_separator_ = separator;
}
