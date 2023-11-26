#include "pico_langtons_ant.hpp"

bool g_isRunning = false;
bool g_isSimuStarted = false;

void task_main() {
  while(true) {
    if (digitalRead(PIN_BUTTON) == LOW) {
      sleep_ms(20);
      if (digitalRead(PIN_BUTTON) == LOW) {
        g_isRunning = !g_isRunning;
        if(g_isRunning && !g_isSimuStarted) {
          tft.fillScreen(DISPLAY_BACK_COLOR);
          g_isSimuStarted = true;
        }
      }
      while(digitalRead(PIN_BUTTON) == LOW);
    }

    if(g_parameterUpdated) {
      g_parameterUpdated = false;

      sem_acquire_timeout_ms(&g_semaphore, TIMEOUT_MS_SEMAPHORE);
      g_langtonsAnt.setParameter(g_parameter_temp);
      sem_release(&g_semaphore);

      g_langtonsAnt.reset();
      tft.fillScreen(DISPLAY_BACK_COLOR);

      g_isRunning = true;
      g_isSimuStarted = true;
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

  tft.setTextColor(DISPLAY_FORE_COLOR, DISPLAY_BACK_COLOR);
  tft.setTextSize(2);
  
  tft.setCursor(30, 10);
  tft.println("Langton's Ant");
  tft.setCursor(30, 40);
  tft.print("WiFi... ");

  WiFi.mode(WIFI_STA);
  ulong current = millis();
  WiFi.begin(STASSID, STAPSK);
  while(millis() - current < TIMEOUT_MS_WIFICONN && WL_CONNECTED !=  WiFi.status()) {
    delay(500);
  }

  if(WL_CONNECTED == WiFi.status()) {
    tft.setTextColor(DISPLAY_OK_FORE_COLOR, DISPLAY_BACK_COLOR);
    tft.print("OK");

    tft.setTextColor(DISPLAY_FORE_COLOR, DISPLAY_BACK_COLOR);
    tft.setCursor(30, 70);
    tft.print(WiFi.localIP().toString());
    tft.printf(":%d\n", SERVER_PORT);

    g_server.on("/parameter", handleParameter);
    g_server.begin();
  }
  else {
    tft.setTextColor(DISPLAY_NG_FORE_COLOR, DISPLAY_BACK_COLOR);
    tft.print("NG");

    tft.setTextColor(DISPLAY_FORE_COLOR, DISPLAY_BACK_COLOR);
    tft.setCursor(30, 70);
    tft.println("Stand-alone mode");
    WiFi.disconnect(true);
  }

  initializeColorPalette(CELL_COLORS_START, CELL_COLORS_MID, CELL_COLORS_END);
  showColorPalette(30, 110, DISPLAY_WIDTH - 30*2);

  sem_init(&g_semaphore, 0, 1);

  multicore_launch_core1(task_main);
}

void loop() {
  if(WiFi.connected()) {
    g_server.handleClient();
  }
}
