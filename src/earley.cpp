#include "../include/earley.h"

void Earley::Predict(const Grammar& grammar, size_t j) {
  for (auto& situation : sit_to_update) {
    if (situation.point_before < situation.right_part.size() &&
        grammar.supportive_.find(situation.right_part[situation.point_before]) != grammar.supportive_.end()) {
      char not_terminal = situation.right_part[situation.point_before];
      for (auto& rule : grammar.rules_.at(not_terminal)) {
        sit_predicted.emplace(not_terminal, rule, 0, j);
      }
    }
  }
}

void Earley::Complete(const Grammar& grammar) {
  for (auto& situation : sit_to_update) {
    if (situation.point_before == situation.right_part.size()) {
      char not_terminal = situation.left_part;
      for (auto& elem : situations_[situation.begin]) {
        if (elem.point_before < elem.right_part.size() && not_terminal == elem.right_part[elem.point_before]) {
          sit_completed.emplace(elem.left_part, elem.right_part, elem.point_before + 1, elem.begin);
        }
      }
    }
  }
}

void Earley::Scan(const Grammar& grammar, size_t j, const std::string& word) {
  if (j == 0) {
    return;
  }
  for (auto& situation : situations_[j - 1]) {
    if (situation.point_before < situation.right_part.size() && situation.right_part[situation.point_before] == word[j - 1]) {
      situations_[j].emplace(situation.left_part, situation.right_part, situation.point_before + 1, situation.begin);
    }
  }
}

bool Earley::CheckWord(const Grammar& grammar, const std::string& word) {
  situations_.resize(word.size() + 1);
  auto empty_elem = situations_[0];
  std::string start_right_part;
  start_right_part += grammar.start_symbol_;
  situations_[0].emplace(zero_symbol_, std::move(start_right_part), 0, 0);
  for (size_t j = 0; j <= word.size(); ++j) {
    Scan(grammar, j, word);
    sit_to_update = situations_[j];
    bool changes = true;
    while (changes) {
      Complete(grammar);
      Predict(grammar, j);
      sit_to_update.clear();
      for (const auto& elem : sit_completed) {
        situations_[j].emplace(elem);
        sit_to_update.emplace(elem);
      }
      for (const auto& elem : sit_predicted) {
        situations_[j].emplace(elem);
        sit_to_update.emplace(elem);
      }
      if (sit_predicted.empty() && sit_completed.empty()) {
        changes = false;
      }
      sit_completed.clear();
      sit_predicted.clear();
    }
  }
  std::string start_right_part_extra;
  start_right_part_extra += grammar.start_symbol_;
  bool word_existence = situations_[word.size()].find(Situation{zero_symbol_, start_right_part_extra, 1, 0}) !=
                        situations_[word.size()].end();
  situations_.clear();
  return word_existence;
}
