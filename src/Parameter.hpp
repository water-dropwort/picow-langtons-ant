#ifndef PARAMETER_HPP_
#define PARAMETER_HPP_

#include <array>
#include "Consts.hpp"
#include "Ant.hpp"
#include "Behaviour.hpp"

struct Parameter {
  std::array<Ant, MAX_ANT_COUNT> initialAnts;
  uint8_t antCount = 1;
  std::array<Behaviour, MAX_RULE_LENGTH> rule;
  uint8_t ruleLength = 1;
};

void copyParameter(const Parameter& src,
                   Parameter& dst) {
  dst.antCount = src.antCount;
  std::copy(src.initialAnts.begin(),
            src.initialAnts.end(),
            dst.initialAnts.begin());
  dst.ruleLength = src.ruleLength;
  std::copy(src.rule.begin(),
            src.rule.end(),
            dst.rule.begin());
}

#endif
