#ifndef ANT_HPP_
#define ANT_HPP_

#include "Coord.hpp"
#include "Direction.hpp"
#include "Consts.hpp"

struct Ant {
  Coord position = {0, 0};
  Direction direction = Direction::North;
};

void moveToNextCell(Ant& ant,
                    int16_t maxHeight,
                    int16_t maxWidth) {
  switch(ant.direction) {
  case Direction::North:
    ant.position.y = (ant.position.y == 0) ?
      maxHeight - 1 : ant.position.y - 1;
    break;
  case Direction::East:
    ant.position.x = (ant.position.x + 1) % maxWidth;
    break;
  case Direction::South:
    ant.position.y = (ant.position.y + 1) % maxHeight;
    break;
  case Direction::West:
    ant.position.x = (ant.position.x == 0) ?
      maxWidth - 1 : ant.position.x - 1;
    break;
  }
}

#endif
