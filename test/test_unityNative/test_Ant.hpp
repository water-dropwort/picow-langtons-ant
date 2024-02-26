#include "unity.h"
#include "Ant.hpp"

#define BOARD_WIDTH  20
#define BOARD_HEIGHT 30

void test_function_moveToNextCell_North() {
  Ant ant;
  ant.direction = Direction::North;
  const int16_t _x = BOARD_WIDTH / 2;
  ant.position = { _x, 0 };
  moveToNextCell(ant, BOARD_HEIGHT, BOARD_WIDTH);
  TEST_ASSERT(ant.position.x == _x);
  TEST_ASSERT(ant.position.y == (BOARD_HEIGHT - 1));
  moveToNextCell(ant, BOARD_HEIGHT, BOARD_WIDTH);
  TEST_ASSERT(ant.position.x == _x);
  TEST_ASSERT(ant.position.y == (BOARD_HEIGHT - 2));
}

void test_function_moveToNextCell_East() {
  Ant ant;
  ant.direction = Direction::East;
  const int16_t _y = BOARD_HEIGHT / 2;
  ant.position = { BOARD_WIDTH - 1, _y };
  moveToNextCell(ant, BOARD_HEIGHT, BOARD_WIDTH);
  TEST_ASSERT(ant.position.x == 0);
  TEST_ASSERT(ant.position.y == _y);
  moveToNextCell(ant, BOARD_HEIGHT, BOARD_WIDTH);
  TEST_ASSERT(ant.position.x == 1);
  TEST_ASSERT(ant.position.y == _y);
}

void test_function_moveToNextCell_South() {
  Ant ant;
  ant.direction = Direction::South;
  const int16_t _x = BOARD_WIDTH / 2;
  ant.position = { _x, (BOARD_HEIGHT - 1) };
  moveToNextCell(ant, BOARD_HEIGHT, BOARD_WIDTH);
  TEST_ASSERT(ant.position.x == _x);
  TEST_ASSERT(ant.position.y == 0);
  moveToNextCell(ant, BOARD_HEIGHT, BOARD_WIDTH);
  TEST_ASSERT(ant.position.x == _x);
  TEST_ASSERT(ant.position.y == 1);
}

void test_function_moveToNextCell_West() {
  Ant ant;
  ant.direction = Direction::West;
  const int16_t _y = BOARD_HEIGHT / 2;
  ant.position = { 0, _y };
  moveToNextCell(ant, BOARD_HEIGHT, BOARD_WIDTH);
  TEST_ASSERT(ant.position.x == (BOARD_WIDTH - 1));
  TEST_ASSERT(ant.position.y == _y);
  moveToNextCell(ant, BOARD_HEIGHT, BOARD_WIDTH);
  TEST_ASSERT(ant.position.x == (BOARD_WIDTH - 2));
  TEST_ASSERT(ant.position.y == _y);
}

