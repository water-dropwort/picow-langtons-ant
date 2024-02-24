#ifndef DIRECTION_HPP_
#define DIRECTION_HPP_

#include "Behaviour.hpp"

enum class Direction{
  North = 0,
  East  = 1,
  South = 2,
  West  = 3
};

Direction newDirection(Direction currentDirection,
                       Behaviour behaviour)
{
  int iCurrentDirection = static_cast<int>(currentDirection);
  switch(behaviour) {
  case Behaviour::Right:
    return static_cast<Direction>((iCurrentDirection + 1) % 4);
  case Behaviour::UTurn:
    return static_cast<Direction>((iCurrentDirection + 2) % 4);
  case Behaviour::Left:
    return static_cast<Direction>((iCurrentDirection + 3) % 4);
  default:
    return currentDirection;
  };
};

#endif
