#pragma once

#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <pico/sem.h>
#include <pico/multicore.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>
#include <ArduinoJson.h>

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
#define MAX_RULE_LENGTH 20

#define STASSID xxx
#define STAPSK  xxx

#define DISPLAY_FORE_COLOR ST77XX_WHITE
#define DISPLAY_OK_FORE_COLOR ST77XX_GREEN
#define DISPLAY_NG_FORE_COLOR ST77XX_RED

#define TIMEOUT_MS_WIFICONN 30000
#define TIMEOUT_MS_SEMAPHORE 5000

Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);

uint16_t g_color_palette[MAX_RULE_LENGTH];
#define CELL_COLORS_START ST77XX_MAGENTA
#define CELL_COLORS_MID ST77XX_GREEN
#define CELL_COLORS_END ST77XX_YELLOW

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

const Ant defaultAnt() {
  Ant ant;
  ant.direction = North;
  ant.position = { DISPLAY_WIDTH / 2,  DISPLAY_HEIGHT / 2 };
  return ant;
}

struct Status {
  uint8_t matrix[DISPLAY_WIDTH][DISPLAY_HEIGHT];
  Ant ants[MAX_ANT_COUNT];
  Vector updated_cell_indexes[MAX_ANT_COUNT];
};

struct Parameter {
  Ant initial_ants[MAX_ANT_COUNT];
  uint8_t ant_count;
  Behaviour rule[MAX_RULE_LENGTH];
  uint8_t rule_length;
};

const Parameter defaultParameter(){
  Parameter parameter;
  for(int i = 0; i < MAX_ANT_COUNT; i++){
      parameter.initial_ants[i] = defaultAnt();
    }
  parameter.ant_count = 1;
  for(int i = 0; i < MAX_RULE_LENGTH; i++) {
    parameter.rule[i] = None;
  }
  parameter.rule[0] = Right;
  parameter.rule[1] = Left;
  parameter.rule_length = 2;
  return parameter;
};

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

Direction resolveNewDirection(Direction current_direction, Behaviour behaviour) {
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

class LangtonsAnt {
private:
  Parameter m_parameter;
  Status m_status;

  void initMatrix(){
    for(int i = 0; i < DISPLAY_WIDTH; i++){
      for(int j = 0; j < DISPLAY_HEIGHT; j++){
        m_status.matrix[i][j] = 0;
      }
    }
  }

public:
  void reset() {
    initMatrix();
    for(int i = 0; i < MAX_ANT_COUNT; i++) {
      if(i < m_parameter.ant_count)
        m_status.ants[i] = m_parameter.initial_ants[i];
      else
        m_status.ants[i] = defaultAnt();
      m_status.updated_cell_indexes[i] = {0,0};
    }
  }

  void updateMatrix(){
    uint8_t cellValue;
    for(int i = 0; i < m_parameter.ant_count; i++) {
      cellValue = m_status.matrix[m_status.ants[i].position.x][m_status.ants[i].position.y];
      m_status.ants[i].direction = resolveNewDirection(m_status.ants[i].direction, m_parameter.rule[cellValue]);
      m_status.matrix[m_status.ants[i].position.x][m_status.ants[i].position.y] = (cellValue + 1) % m_parameter.rule_length;
      m_status.updated_cell_indexes[i] = m_status.ants[i].position;
      moveAnt(&m_status.ants[i]);
    }
  }

  void setParameter(const Parameter& new_parameter) {
    m_parameter = new_parameter;
  }

  const Parameter& getParameter() const {
    return m_parameter;
  }

  const Status &getStatus() const { return m_status; }
};

LangtonsAnt g_langtonsAnt;

void initializeColorPalette(uint16_t start_color, uint16_t mid_color, uint16_t end_color) {
  uint16_t start_r = (start_color >> 11) & 0x1F;
  uint16_t start_g = (start_color >> 5) & 0x3F;
  uint16_t start_b = start_color & 0x1F;
  Serial.printf("Start R:%d,G:%d,B:%d\n", start_r, start_g, start_b);
  uint16_t mid_r = (mid_color >> 11) & 0x1F;
  uint16_t mid_g = (mid_color >> 5) & 0x3F;
  uint16_t mid_b = mid_color & 0x1F;
  Serial.printf("Mid R:%d,G:%d,B:%d\n", mid_r, mid_g, mid_b);
  uint16_t end_r = (end_color >> 11) & 0x1F;
  uint16_t end_g = (end_color >> 5) & 0x3F;
  uint16_t end_b = end_color & 0x1F;
  Serial.printf("End R:%d,G:%d,B:%d\n", end_r, end_g, end_b);
  float step_r_sm = (float)(mid_r - start_r) / ((MAX_RULE_LENGTH - 1) / 2);
  float step_g_sm = (float)(mid_g - start_g) / ((MAX_RULE_LENGTH - 1) / 2);
  float step_b_sm = (float)(mid_b - start_b) / ((MAX_RULE_LENGTH - 1) / 2);
  Serial.printf("Step R:%f,G:%f,B:%f\n", step_r_sm, step_g_sm, step_b_sm);
  float step_r_me = (float)(end_r - mid_r) / ((MAX_RULE_LENGTH - 1) / 2);
  float step_g_me = (float)(end_g - mid_g) / ((MAX_RULE_LENGTH - 1) / 2);
  float step_b_me = (float)(end_b - mid_b) / ((MAX_RULE_LENGTH - 1) / 2);
  Serial.printf("Step R:%f,G:%f,B:%f\n", step_r_me, step_g_me, step_b_me);

  g_color_palette[0] = DISPLAY_BACK_COLOR;
  uint16_t r = 0;
  uint16_t g = 0;
  uint16_t b = 0;
  for(int i = 1; i < MAX_RULE_LENGTH / 2; i++) {
    r = (start_r + step_r_sm * (i-1));
    g = (start_g + step_g_sm * (i-1));
    b = (start_b + step_b_sm * (i-1));
    g_color_palette[i] = ((r & 0x1F) << 11)| ((g & 0x3F) << 5) | (b & 0x1F);
    Serial.printf("Step[%d] R:%d,G:%d.B:%d\n", i, r, g, b);
  }
  for(int i = MAX_RULE_LENGTH / 2; i < MAX_RULE_LENGTH; i++) {
    r = (mid_r + step_r_me * (i-(MAX_RULE_LENGTH / 2)));
    g = (mid_g + step_g_me * (i-(MAX_RULE_LENGTH / 2)));
    b = (mid_b + step_b_me * (i-(MAX_RULE_LENGTH / 2)));
    g_color_palette[i] = ((r & 0x1F) << 11)| ((g & 0x3F) << 5) | (b & 0x1F);
    Serial.printf("Step[%d] R:%d,G:%d.B:%d\n", i, r, g, b);
  }
}

void showColorPalette(int16_t x, int16_t y, int16_t w) {
  int16_t rect_size = ((w - MAX_RULE_LENGTH * 2) / MAX_RULE_LENGTH);
  for(int i = 0; i < MAX_RULE_LENGTH; i++) {
    tft.fillRect(x + i * (rect_size + 2), y, rect_size, rect_size, g_color_palette[i]);
  }
}

void updateDisplay() {
  for(int i = 0; i < g_langtonsAnt.getParameter().ant_count; i++) {
    int x = g_langtonsAnt.getStatus().updated_cell_indexes[i].x;
    int y = g_langtonsAnt.getStatus().updated_cell_indexes[i].y;
    tft.drawPixel(x, y, g_color_palette[g_langtonsAnt.getStatus().matrix[x][y]]);
  }
}


#define SERVER_PORT 8888

WebServer g_server(SERVER_PORT);
StaticJsonDocument<1024> g_jsondoc;
semaphore_t g_semaphore;

Parameter g_parameter_temp;
bool g_parameterUpdated = false;

void setHeaders() {
  g_server.sendHeader("Access-Control-Allow-Origin", "*");
  g_server.sendHeader("Access-Control-Allow-Credentials","true");
  g_server.sendHeader("Access-Control-Allow-Methods", "POST");
}

void handleParameter() {
  setHeaders();

  if(g_server.method() != HTTP_POST) {
    g_server.send(405, "text/plain", "Method not allowed");
    return;
  }

  if(g_server.hasArg("plain") == false) {
    g_server.send(400, "text/plain", "Bad request");
    return;
  }

  String body = g_server.arg("plain");
  DeserializationError jsonerr = deserializeJson(g_jsondoc, body);
  if(jsonerr) {
    g_server.send(400, "text/plain", String("Bad request:") + jsonerr.c_str());
    return;
  }

  sem_acquire_timeout_ms(&g_semaphore, TIMEOUT_MS_SEMAPHORE);

  g_parameter_temp.ant_count = (uint8_t)g_jsondoc["ants"]["count"].as<uint32_t>();
  JsonArray jarr_ants_params = g_jsondoc["ants"]["params"].as<JsonArray>();
  int antNo = 0;
  for(JsonVariant jvar : jarr_ants_params) {
    uint32_t ants_param = jvar.as<uint32_t>();
    g_parameter_temp.initial_ants[antNo].direction = (Direction)(ants_param & 0x3);
    g_parameter_temp.initial_ants[antNo].position.x = (ants_param >> 2) & 0xFF;
    g_parameter_temp.initial_ants[antNo].position.y = (ants_param >> 10) & 0x1FF;
    antNo++;
  }
  g_parameter_temp.rule_length = g_jsondoc["rule"]["length"];
  uint32_t rule = g_jsondoc["rule"]["rule"].as<uint32_t>();
  for(int i = 0; i < g_parameter_temp.rule_length; i++) {
    g_parameter_temp.rule[i] = (Behaviour)((rule >> (2*i)) & 0x3);
  }

  sem_release(&g_semaphore);

  g_parameterUpdated = true;

  g_server.send(200, "text/plain", "OK");
}

void handleNotFound() {
  setHeaders();
  g_server.send(404, "text/plain", "Not Found");
}
