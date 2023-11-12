#pragma once

#include <Adafruit_ST7789.h>
#include <SPI.h>

#define PIN_TFT_CS    17
#define PIN_TFT_RST   22
#define PIN_TFT_DC    28
#define PIN_TFT_MOSI  19
#define PIN_TFT_SCK   18
#define PIN_TFT_BL    16
#define PIN_BUTTON    20
#define PIN_VOLUME    27

#define DISPLAY_HEIGHT 280
#define DISPLAY_WIDTH  240

#define DISPLAY_BACK_COLOR ST77XX_BLACK
#define MAX_ANT_COUNT 10
#define MAX_RULE_LENGTH 10

Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);

// CELL_COLORSの長さはMAX_RULE_LENGTHと等しくなること。
const uint16_t CELL_COLORS[] = {
  DISPLAY_BACK_COLOR,
  0x4000,
  0x6000,
  0x8000,
  0xA000,
  0xC000,
  0xE000,
  0xFC00,
  0xF800,
  0xF000,
};

enum Direction
{
  North,
  East,
  South,
  West
};

enum Behaviour
{
  Left,
  Right,
  None,
  Turn,
};

struct Vector {
  uint16_t x;
  uint16_t y;
};

struct Ant {
  Vector position;
  Direction direction;
};

uint8_t g_matrix[DISPLAY_WIDTH][DISPLAY_HEIGHT];
Ant g_ants[MAX_ANT_COUNT];
bool g_isRunning = true;
uint8_t g_current_ant_count = 1;
Vector g_updated_cell_indexes[MAX_ANT_COUNT];
Behaviour g_rule[MAX_RULE_LENGTH];
uint8_t g_current_rule_length;

void initMatrix(){
  for(int i = 0; i < DISPLAY_WIDTH; i++){
    for(int j = 0; j < DISPLAY_HEIGHT; j++){
      g_matrix[i][j] = 0;
    }
  }
}

void initAnts(){
  for(int i = 0; i < MAX_ANT_COUNT; i++){
    g_ants[i] = Ant();
    g_ants[i].direction = North;
    g_ants[i].position = Vector();
    g_ants[i].position.x = DISPLAY_WIDTH / 2;
    g_ants[i].position.y = DISPLAY_HEIGHT / 2;
  }
}

void initRule() {
  for(int i = 0; i < MAX_RULE_LENGTH; i++) {
    g_rule[i] = None;
  }
  g_rule[0] = Right;
  g_rule[1] = Left;
  g_current_rule_length = 2;
}

Direction changeDirection(Direction current_direction, Behaviour behaviour) {
  if(behaviour == Right) {
    switch(current_direction) {
      case North: return East;
      case East:  return South;
      case South: return West;
      case West:  return North;
      default:    return current_direction;
    }
  }
  else if(behaviour == Left) {
    switch(current_direction) {
      case North: return West;
      case East:  return North;
      case South: return East;
      case West:  return South;
      default:    return current_direction;
    }
  }
  else if(behaviour == Turn) {
    switch(current_direction) {
      case North: return South;
      case East:  return West;
      case South: return North;
      case West:  return East;
      default:    return current_direction;
    }
  }
  else {
    return current_direction;
  }
}

void moveAnt(Ant *ant) {
  switch(ant->direction) {
    case North:
      if(ant->position.y == 0)
        ant->position.y = DISPLAY_HEIGHT - 1;
      else
        ant->position.y -= 1;
      break;
    case East:
      ant->position.x = (ant->position.x + 1) % DISPLAY_WIDTH;
      break;
    case South:
      ant->position.y = (ant->position.y + 1) % DISPLAY_HEIGHT;
      break;
    case West:
      if(ant->position.x == 0)
        ant->position.x = DISPLAY_WIDTH - 1;
      else
        ant->position.x -= 1;
      break;
  }
}

void updateMatrix(){
  uint8_t cellValue;
  for(int i = 0; i < g_current_ant_count; i++) {
    cellValue = g_matrix[g_ants[i].position.x][g_ants[i].position.y];
    g_ants[i].direction = changeDirection(g_ants[i].direction, g_rule[cellValue]);
    g_matrix[g_ants[i].position.x][g_ants[i].position.y] = (cellValue + 1) % g_current_rule_length;
    g_updated_cell_indexes[i] = g_ants[i].position;
    moveAnt(&g_ants[i]);
  }
}

void updateDisplay() {
  for(int i = 0; i < g_current_ant_count; i++) {
    int x = g_updated_cell_indexes[i].x;
    int y = g_updated_cell_indexes[i].y;
    tft.drawPixel(x, y, CELL_COLORS[g_matrix[x][y]]);
  }
}
