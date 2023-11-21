#include "pico_langtons_ant.hpp"

bool g_isRunning = true;

void task_main() {
  while(true) {
    if (digitalRead(PIN_BUTTON) == LOW) {
      sleep_ms(20);
      if (digitalRead(PIN_BUTTON) == LOW) {
        g_isRunning = !g_isRunning;
      }
      while(digitalRead(PIN_BUTTON) == LOW);
    }

    if(g_parameterUpdated) {
      g_parameterUpdated = false;

      sem_acquire_timeout_ms(&g_semaphore, 5000);
      g_langtonsAnt.setParameter(g_parameter_temp);
      sem_release(&g_semaphore);

      g_langtonsAnt.reset();
      tft.fillScreen(DISPLAY_BACK_COLOR);

      g_isRunning = true;
    }

    if(g_isRunning) {
      g_langtonsAnt.updateMatrix();
      updateDisplay();
      sleep_us(map(analogRead(PIN_VOLUME), 0, 1023, 100, 1000));
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  tft.init(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  tft.setRotation(0);
  tft.fillScreen(DISPLAY_BACK_COLOR);

  g_langtonsAnt.setParameter(defaultParameter());
  g_langtonsAnt.reset();

  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  g_server.on("/parameter", handleParameter);
  g_server.begin();

  sem_init(&g_semaphore, 0, 1);

  multicore_launch_core1(task_main);
}

void loop() {
  if(WiFi.connected()) {
    g_server.handleClient();
  }
}
