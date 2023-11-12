#include "pico_langtons_ant.hpp"


void setup()
{
  Serial.begin();
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  tft.init(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  tft.setRotation(0);
  initMatrix();
  tft.fillScreen(DISPLAY_BACK_COLOR);
  initAnts();
  initRule();
}

void loop()
{
  if (digitalRead(PIN_BUTTON) == LOW) {
    sleep_ms(20);
    if (digitalRead(PIN_BUTTON) == LOW) {
      g_isRunning = !g_isRunning;
    }
    while(digitalRead(PIN_BUTTON) == LOW);
  }
  if(g_isRunning)
  {
    updateMatrix();
    updateDisplay();
    sleep_us(map(analogRead(PIN_VOLUME), 0, 1023, 100, 1000));
  }
}
