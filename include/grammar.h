#include <map>
#include <set>
#include <string>
#include <vector>

#ifndef AUTOMATONS__GRAMMAR_H_
#define AUTOMATONS__GRAMMAR_H_
#pragma once

class Earley;

class Grammar {
 public:
  Grammar() = default;
  explicit Grammar(const std::string&, bool&);
  bool AddRule(const std::string&);
  void AddTerminal(const char&);
  void AddSupportive(const char&);
  void SetInRuleSeparator(const char&);
  friend class Earley;

 private:
  std::set<char> terminals_;
  std::set<char> supportive_;
  char start_symbol_ = 'S';
  std::map<char, std::vector<std::string>> rules_;
  char in_rule_separator_ = '>';
};

#endif  // AUTOMATONS__GRAMMAR_H_