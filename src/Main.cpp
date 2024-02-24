#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <pico/multicore.h>
#include "LangtonsAnt.hpp"
#include "RenderedGrid.hpp"
#include "Server.hpp"
#include "Consts.hpp"

// Application status
enum class AppState {
  Init,
  Running,
  Stop,
};

AppState g_appState = AppState::Init;
bool g_parameterUpdated = false;
semaphore_t g_semaphore;
Adafruit_ST7789* g_st7789;
RenderedGrid<DISPLAY_WIDTH, DISPLAY_HEIGHT>* g_grid;
LangtonsAnt* g_langtonsAnt = nullptr;
LangtonsAntServer* g_server = nullptr;

void task_langtonsAntSimulation();
void printOpeningMsg();
void printSuccWiFiConnMsg(const char* ipAddress,
                          int port);
void printFailWiFiConnMsg();
void drawColorPalette();

void setup() {
  // === INITIALIZE ===
  Serial.begin(PICO_DEFAULT_UART_BAUD_RATE);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  // Initialize display driver.
  g_st7789 = new Adafruit_ST7789(&SPI, PIN_TFT_CS,
                                 PIN_TFT_DC, PIN_TFT_RST);
  g_st7789->init(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  g_st7789->setRotation(0);
  g_st7789->fillScreen(COLOR_BACKGROUND);
  // Create Renderer.
  g_grid = new RenderedGrid<DISPLAY_WIDTH,
                            DISPLAY_HEIGHT>(*g_st7789);
  // Create Simulator.
  g_langtonsAnt = new LangtonsAnt(*g_grid);
  // Initialize semaphore.
  sem_init(&g_semaphore, 0, 1);

  // === OPENING SCREEN ===
  printOpeningMsg();
  // WiFi connecting
  WiFi.mode(WIFI_STA);
  const ulong current = millis();
  WiFi.begin(STASSID, STAPSK);
  while(millis() - current < TIMEOUT_MS_WIFICONN &&
        WL_CONNECTED !=  WiFi.status())
  {
    delay(500);
  }
  // Print wifi connecting status message.
  if(WL_CONNECTED == WiFi.status()) {
    printSuccWiFiConnMsg(WiFi.localIP().toString().c_str(),
                         SERVER_PORT);
    g_server = new LangtonsAntServer(&g_semaphore);
    g_server->SetFunc_OnConfigUpdated([](){
      g_parameterUpdated = true;
    });
    g_server->begin();
  }
  else {
    printFailWiFiConnMsg();
    WiFi.disconnect(true);
  }
  // Draw color palette
  drawColorPalette();

  // Start simulator task on core1.
  multicore_launch_core1(task_langtonsAntSimulation);
}

void loop() {
  if(WiFi.connected()) {
    g_server->HandleClient();;
  }
}

void task_langtonsAntSimulation() {
  while(true) {
    if (digitalRead(PIN_BUTTON) == LOW) {
      sleep_ms(20);
      if (digitalRead(PIN_BUTTON) == LOW) {
        switch(g_appState) {
        case AppState::Init:
          g_grid->clearStates();
          g_langtonsAnt->resetAnts();
          g_appState = AppState::Running;
          break;
        case AppState::Running:
          g_appState = AppState::Stop;
          break;
        case AppState::Stop:
          g_appState = AppState::Running;
          break;
        }
      }
      while(digitalRead(PIN_BUTTON) == LOW);
    }

    if(g_parameterUpdated) {
      g_parameterUpdated = false;

      // Set config to simulator.
      sem_acquire_timeout_ms(&g_semaphore, TIMEOUT_MS_SEMAPHORE);
      g_langtonsAnt->setParameter(g_server->GetParameter());
      sem_release(&g_semaphore);

      g_grid->clearStates();
      g_langtonsAnt->resetAnts();
      g_appState = AppState::Running;
    }

    if(g_appState == AppState::Running) {
      g_langtonsAnt->update();
      sleep_us(analogRead(PIN_VOLUME) + 1);
    }
  }
}

void printOpeningMsg() {
  g_st7789->setTextColor(COLOR_FOREGROUND, COLOR_BACKGROUND);
  g_st7789->setTextSize(2);
  g_st7789->setCursor(30, 10);
  g_st7789->println("Langton's Ant");
  g_st7789->setCursor(30, 40);
  g_st7789->print("WiFi...");
}

void printSuccWiFiConnMsg(const char* ipAddress, int port) {
  g_st7789->setTextColor(COLOR_FOREGROUND_OK, COLOR_BACKGROUND);
  g_st7789->print("OK");
  g_st7789->setTextColor(COLOR_FOREGROUND, COLOR_BACKGROUND);
  g_st7789->setCursor(30, 70);
  g_st7789->printf("%s:%d\n", ipAddress, port);
}

void printFailWiFiConnMsg() {
  g_st7789->setTextColor(COLOR_FOREGROUND_NG, COLOR_BACKGROUND);
  g_st7789->print("NG");
  g_st7789->setTextColor(COLOR_FOREGROUND, COLOR_BACKGROUND);
  g_st7789->setCursor(30, 70);
  g_st7789->println("Stand-alone mode");
}

void drawColorPalette() {
  int16_t x = 30;
  int16_t y = 110;
  int16_t rectSize =
    (DISPLAY_WIDTH - x * 2) / MAX_RULE_LENGTH;
  for(int i =0; i < MAX_RULE_LENGTH; i++) {
    g_st7789->fillRect(x + i * rectSize, y,
                       rectSize - 1, rectSize,
                       g_grid->getColorFromPalette(i));
    // g_st7789->drawRect(x + i * rectSize, y,
    //                    rectSize, rectSize,
    //                    COLOR_FOREGROUND);
  }
}
