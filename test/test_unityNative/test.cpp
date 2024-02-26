#include "unity.h"
#include "LangtonsAnt.hpp"
#include "test_Direction.hpp"
#include "test_Ant.hpp"

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_function_newDirection);
  RUN_TEST(test_function_moveToNextCell_North);
  RUN_TEST(test_function_moveToNextCell_East);
  RUN_TEST(test_function_moveToNextCell_South);
  RUN_TEST(test_function_moveToNextCell_West);
  return UNITY_END();;
}
