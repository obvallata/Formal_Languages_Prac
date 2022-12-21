#include <unordered_set>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <fstream>

#ifndef CYK_ALGORITHM__GRAMMAR_H_
#define CYK_ALGORITHM__GRAMMAR_H_
#pragma once

using rules_list_type = std::set<std::pair<std::string, std::vector<std::string>>>;
class ChomskyNormalForm;
class Grammar {
 public:
  explicit Grammar(std::basic_fstream<char>&);

  const std::unordered_set<std::string>& GetTerminals() const {
    return terminals_;
  }

  const std::unordered_set<std::string>& GetNonTerminals() const {
    return non_terminals_;
  }

  const rules_list_type& GetRules() const {
    return rules_;
  }

  const std::string& GetStartSymbol() const {
    return start_sym_;
  }

  char GetSeparator() const {
    return separator_;
  }

  char GetBorder() const {
    return sym_border_;
  }

  bool CYK(const std::string&);

 private:
  void FillDp(std::map<std::string, std::vector<std::vector<bool>>>&, size_t, size_t);
  void AddTerminal(const std::string&);
  void AddNonTerminal(const std::string&);
  void AddRule(const std::string&);
  void SetStartSymbol(const std::string&);
  void SetInRuleSeparator(const char&);
  void SetSymBorder(const char&);

  friend ChomskyNormalForm;
  std::unordered_set<std::string> terminals_;
  std::unordered_set<std::string> non_terminals_;
  rules_list_type rules_;
  std::string start_sym_ = "S";
  char separator_ = '>';
  char sym_border_ = ':';
  const std::string very_start_symbol_ = "$";
};

class ChomskyNormalForm {
 public:
  static Grammar MakeChomskyNormalForm(const Grammar&);

 private:
  static bool RemoveEpsGen(Grammar&);
  static void RemoveNonGen(Grammar&);
  static void RemoveMixedRules(Grammar&);
  static void RemoveLongRules(Grammar&);
  static void RemoveUnaryRules(Grammar&);
};

#endif //CYK_ALGORITHM__GRAMMAR_H_
