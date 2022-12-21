#include <algorithm>
#include "../include/grammar.h"
#include <fstream>
#include <sstream>
#include <map>

template <typename T>
std::string ToString(T val) {
  std::ostringstream oss;
  oss << val;
  return oss.str();
}

void Grammar::AddTerminal(const std::string& term) {
  terminals_.emplace(term);
}

void Grammar::AddNonTerminal(const std::string& non_term) {
  non_terminals_.emplace(non_term);
}

void Grammar::AddRule(const std::string& rule) {
  auto sep_pos = std::find(rule.begin(), rule.end(), separator_);
  std::string left = rule.substr(0, std::abs(std::distance(rule.begin(), sep_pos)));
  std::vector<std::string> right;
  auto right_substr = rule.substr(std::abs(std::distance(rule.begin(), sep_pos)) + 1);
  auto begin_sym = sep_pos + 1;
  auto curr_border = std::find(std::next(sep_pos), rule.end(), sym_border_);
  while (curr_border != rule.end()) {
    right.emplace_back(rule.substr(std::abs(std::distance(begin_sym, rule.begin())), std::abs(std::distance(curr_border, begin_sym))));
    begin_sym = std::next(curr_border);
    curr_border = std::find(std::next(curr_border), rule.end(), sym_border_);
  }
  right.emplace_back(rule.substr(std::abs(std::distance(begin_sym, rule.begin())), std::abs(std::distance(curr_border, begin_sym))));
  rules_.emplace(std::pair<std::string, std::vector<std::string>>{left, right});
}

void Grammar::SetStartSymbol(const std::string& sym) {
  start_sym_ = sym;
}

void Grammar::SetInRuleSeparator(const char& sep) {
  separator_ = sep;
}

void Grammar::SetSymBorder(const char& sym) {
  sym_border_ = sym;
}

Grammar::Grammar(std::basic_fstream<char>& input) {
  std::string line;
  std::getline(input, line);
  SetStartSymbol(line);
  AddNonTerminal(line);
  std::getline(input, line);
  while (line != "---") {
    AddNonTerminal(line);
    std::getline(input, line);
  }
  std::getline(input, line);
  for (auto& sym : line) {
    std::string sym_line;
    sym_line += sym;
    AddTerminal(sym_line);
  }
  std::getline(input, line);
  SetInRuleSeparator(line[0]);
  std::getline(input, line);
  SetSymBorder(line[0]);
  while(std::getline(input, line)) {
    AddRule(line);
  }
  input.close();
}

Grammar ChomskyNormalForm::MakeChomskyNormalForm(const Grammar& grammar) {
  Grammar chomsky_grammar = grammar;
  bool start_gen_eps = RemoveEpsGen(chomsky_grammar);
  chomsky_grammar.AddNonTerminal(chomsky_grammar.very_start_symbol_);
  std::string new_rule;
  new_rule += chomsky_grammar.very_start_symbol_;
  new_rule += chomsky_grammar.separator_;
  new_rule += chomsky_grammar.start_sym_;
  chomsky_grammar.AddRule(new_rule);
  chomsky_grammar.start_sym_ = chomsky_grammar.very_start_symbol_;
  RemoveNonGen(chomsky_grammar);
  RemoveMixedRules(chomsky_grammar);
  RemoveLongRules(chomsky_grammar);
  RemoveEpsGen(chomsky_grammar);
  if (start_gen_eps) {
    auto rule = *chomsky_grammar.rules_.begin();
    rule.first = chomsky_grammar.start_sym_;
    rule.second.clear();
    rule.second.emplace_back("");
    chomsky_grammar.rules_.emplace(rule);
  }
  RemoveUnaryRules(chomsky_grammar);
  return chomsky_grammar;
}

void AddExtraRules(rules_list_type& rules, const std::pair<std::string, std::vector<std::string>>& curr_rule, const std::unordered_set<std::string>& eps_gen) {
  if (curr_rule.second.size() == 1 && curr_rule.second[0].empty()) {
    return;
  }
  rules_list_type new_rules;
  new_rules.emplace(curr_rule);
  size_t count_eps_gen = 0;
  for (auto& sym : curr_rule.second) {
    if (eps_gen.find(sym) != eps_gen.end()) {
      count_eps_gen++;
    }
  }
  for (size_t i = 0; i < count_eps_gen; ++i)  {
    auto clone_new_rules = new_rules;
    for (auto& rule : clone_new_rules) {
      for (size_t j = 0; j < rule.second.size(); ++j) {
        if (eps_gen.find(rule.second[j]) != eps_gen.end()) {
          auto rule_to_add = rule;
          rule_to_add.second.clear();
          for (size_t k = 0; k < rule.second.size(); ++k) {
            if (k != j) {
              rule_to_add.second.emplace_back(rule.second[k]);
            }
          }
          if (!rule_to_add.second.empty()) {
            new_rules.emplace(rule_to_add);
          }
        }
      }
    }
  }
  for (auto& elem : new_rules) {
    rules.emplace(elem);
  }
}

bool ChomskyNormalForm::RemoveEpsGen(Grammar& grammar) {
  bool start_eps_gen = false;
  std::unordered_set<std::string> eps_gen;
  for (auto& rule : grammar.rules_) {
    if (std::any_of(rule.second.begin(), rule.second.end(), [](const std::string& right){ return right.empty(); })) {
      eps_gen.emplace(rule.first);
    }
  }
  auto clone = eps_gen;
  do {
    clone = eps_gen;
    for (auto& rule : grammar.rules_) {
      if (eps_gen.find(rule.first) == eps_gen.end()) {
        if (std::all_of(rule.second.begin(), rule.second.end(), [&eps_gen](const std::string& sym){ return eps_gen.find(sym) != eps_gen.end(); })) {
          eps_gen.emplace(rule.first);
        }
      }
    }
  } while (clone != eps_gen);
  if (eps_gen.find(grammar.start_sym_) != eps_gen.end()) {
    start_eps_gen = true;
  }
  rules_list_type new_rules;
  for (auto& rule : grammar.rules_) {
    AddExtraRules(new_rules, rule, eps_gen);
  }
  grammar.rules_ = new_rules;
  return start_eps_gen;
}

void ChomskyNormalForm::RemoveNonGen(Grammar& grammar) {
  std::unordered_set<std::string> gen;
  gen.emplace(grammar.start_sym_);
  std::unordered_set<std::string> sym_to_check;
  sym_to_check.emplace(grammar.start_sym_);
  while(!sym_to_check.empty()) {
    auto curr_sym = *sym_to_check.begin();
    sym_to_check.erase(sym_to_check.begin());
    for (auto& rule : grammar.rules_) {
      if (rule.first != curr_sym) {
        continue;
      }
      for (auto& sym : rule.second) {
        if (grammar.non_terminals_.find(sym) != grammar.non_terminals_.end() && gen.find(sym) == gen.end()) {
          gen.emplace(sym);
          sym_to_check.emplace(sym);
        }
      }
    }
  }
  rules_list_type new_rules;
  for (const auto& rule : grammar.rules_) {
    if (gen.find(rule.first) != gen.end()) {
      new_rules.emplace(rule);
    }
  }
  grammar.non_terminals_ = gen;
  grammar.rules_ = new_rules;
}

void ChomskyNormalForm::RemoveMixedRules(Grammar& grammar) {
  rules_list_type new_rules;
  for (auto& rule : grammar.rules_) {
    auto rule_clone = rule;
    for (auto& sym : rule_clone.second) {
      if (grammar.terminals_.find(sym) != grammar.terminals_.end() && rule.second.size() > 1) {
        std::string non_term = std::string(ToString(grammar.non_terminals_.size()));
        grammar.non_terminals_.emplace(non_term);
        auto rule_for_letter = rule_clone;
        rule_for_letter.first = non_term;
        rule_for_letter.second.clear();
        rule_for_letter.second.emplace_back(sym);
        new_rules.emplace(rule_for_letter);
        sym = non_term;
      }
    }
    new_rules.emplace(rule_clone);
  }
  grammar.rules_ = new_rules;
}

void ChomskyNormalForm::RemoveLongRules(Grammar& grammar) {
  bool changes = true;
  rules_list_type new_rules;
  while (changes) {
    for (auto& rule : grammar.rules_) {
      auto rule_clone = rule;
      if (rule_clone.second.size() > 2) {
        std::string left = std::string(ToString(grammar.non_terminals_.size()));
        grammar.non_terminals_.emplace(left);
        std::vector<std::string> right;
        for(size_t i = 0; i < rule_clone.second.size() - 1; ++i) {
          right.emplace_back(rule_clone.second[i]);
        }
        auto rule_extra_clone = rule_clone;
        rule_extra_clone.first = left;
        rule_extra_clone.second = right;
        for (size_t i = 0; i < rule.second.size() - 1; ++i) {
          rule_clone.second.erase(rule_clone.second.begin());
        }
        rule_clone.second.insert(rule_clone.second.cbegin(), left);
        new_rules.emplace(rule_extra_clone);
      }
      new_rules.emplace(rule_clone);
    }
    if (grammar.rules_ == new_rules) {
      changes = false;
    } else {
      grammar.rules_ = new_rules;
      new_rules.clear();
    }
  }
}

void ChomskyNormalForm::RemoveUnaryRules(Grammar& grammar) {
  bool changes = true;
  rules_list_type new_rules;
  while (changes) {
    for (auto& rule : grammar.rules_) {
      if (rule.second.size() == 1 && grammar.non_terminals_.find(rule.second[0]) != grammar.non_terminals_.end()) {
        for (auto& rule_extra : grammar.rules_) {
          if (rule_extra.first == rule.second[0]) {
            auto rule_to_add = rule;
            rule_to_add.second = rule_extra.second;
            new_rules.emplace(rule_to_add);
          }
        }
      } else {
        new_rules.emplace(rule);
      }
    }
    if (grammar.rules_ == new_rules) {
      changes = false;
    } else {
      grammar.rules_ = new_rules;
      new_rules.clear();
    }
  }
}



bool Grammar::CYK(const std::string& word) {
  std::map<std::string, std::vector<std::vector<bool>>> dp;
  if (word == "$") {
    for (auto& rule : rules_) {
      if (rule.first == start_sym_ && rule.second.size() == 1 && rule.second[0].empty()) {
        return true;
      }
    }
    return false;
  }
  for (auto& elem : non_terminals_) {
    dp[elem].resize(word.size() + 1, std::vector<bool>(word.size() + 1, false));
    for (size_t i = 0; i < word.size(); ++i) {
      bool letter_exists = false;
      for (auto& rule : rules_) {
        std::string letter;
        letter += word[i];
        if (rule.first == elem && rule.second.size() == 1 && rule.second[0] == letter) {
          letter_exists = true;
        }
        if (letter_exists) {
          break;
        }
      }
      dp[elem][i][i + 1] = letter_exists;
    }
  }
  for (size_t len = 2; len < word.size() + 1; ++len) {
    FillDp(dp, len, word.size());
  }
  return dp[start_sym_][0][word.size()];
}

void Grammar::FillDp(std::map<std::string, std::vector<std::vector<bool>>>& dp, size_t len, size_t len_word) {
  for (size_t start = 0; start < len_word; ++start) {
    size_t finish = len + start;
    finish = std::min(finish, len_word);
    for (auto& rule : rules_) {
      if (rule.second.size() == 1) {
        continue;
      }
      for (size_t mid = start + 1; mid <= finish - 1; ++mid) {
        dp[rule.first][start][finish] = dp[rule.first][start][finish] || (dp[rule.second[0]][start][mid] && dp[rule.second[1]][mid][finish]);
      }
    }
  }
}
