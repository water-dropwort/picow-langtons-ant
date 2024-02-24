#ifndef LANGTONSANT_HPP_
#define LANGTONSANT_HPP_

#include "IGrid.hpp"
#include "Ant.hpp"
#include "Parameter.hpp"
#include "Direction.hpp"
#include "Behaviour.hpp"

class LangtonsAnt {
public:
  LangtonsAnt(IGrid& grid) : m_grid(grid){
    // Default parameter
    m_parameter.antCount = 1;
    m_parameter.initialAnts[0].direction = Direction::North;
    m_parameter.initialAnts[0].position.x = grid.width() / 2;
    m_parameter.initialAnts[0].position.y = grid.height() / 2;
    m_parameter.rule[0] = Behaviour::Right;
    m_parameter.rule[1] = Behaviour::Left;
    m_parameter.ruleLength = 2;
  };

  void update() {
    for(int i = 0; i < m_parameter.antCount; i++) {
      Coord currentPos = m_ants[i].position;
      uint8_t currentState = m_grid.state(currentPos);

      // Change the ant's direction.
      m_ants[i].direction =
        newDirection(m_ants[i].direction,
                     m_parameter.rule[currentState]);
      // Increment the cell's state
      uint8_t nextState
        = (currentState + 1) % m_parameter.ruleLength;
      m_grid.updateState(currentPos, nextState);
      // Move the ant.
      moveToNextCell(m_ants[i], m_grid.height(), m_grid.width());
    }
  }

  void setParameter(const Parameter& newParameter) {
    copyParameter(newParameter, m_parameter);
  }

  void resetAnts() {
    std::copy(m_parameter.initialAnts.begin(),
              m_parameter.initialAnts.end(),
              m_ants.begin());
  }
private:
  IGrid& m_grid;
  Parameter m_parameter;
  std::array<Ant, MAX_ANT_COUNT> m_ants;
};

#endif
