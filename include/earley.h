#include "grammar.h"
#include <string>

#ifndef EARLEY__EARLEY_H_
#define EARLEY__EARLEY_H_
#pragma once

struct Situation {
  char left_part;
  std::string right_part;
  size_t point_before;
  size_t begin;
  Situation(char left_part, std::string right_part, size_t curr_pos, size_t begin)
      : left_part(left_part), right_part(std::move(right_part)), point_before(curr_pos), begin(begin){};
  bool operator==(const Situation& sec) const {
    return (left_part == sec.left_part) && (right_part == sec.right_part) && (point_before == sec.point_before) &&
           (begin == sec.begin);
  }
};

struct SituationComparator {
  bool operator()(const Situation& fir, const Situation& sec) const {
    if (fir.left_part != sec.left_part) {
      return fir.left_part < sec.left_part;
    }
    if (fir.right_part != sec.right_part) {
      return fir.right_part < sec.right_part;
    }
    if (fir.point_before != sec.point_before) {
      return fir.point_before < sec.point_before;
    }
    return fir.begin < sec.begin;
  }
};

using situation_list = std::vector<std::set<Situation, SituationComparator>>;

class Earley {
 public:
  Earley() = default;
  bool CheckWord(const Grammar&, const std::string&);

 private:
  void Predict(const Grammar&, size_t);
  void Complete(const Grammar&);
  void Scan(const Grammar&, size_t, const std::string&);
  situation_list situations_;
  char zero_symbol_ = '$';
  std::set<Situation, SituationComparator> sit_to_update;
  std::set<Situation, SituationComparator> sit_predicted;
  std::set<Situation, SituationComparator> sit_completed;
};

#endif  // EARLEY__EARLEY_H_
