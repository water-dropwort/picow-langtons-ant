#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <pico/sem.h>
#include <functional>
#include "Consts.hpp"
#include "Parameter.hpp"


class LangtonsAntServer {
public:
  LangtonsAntServer(semaphore_t* semaphore) {
    m_server = new WebServer(SERVER_PORT);
    m_semaphore = semaphore;
  }

  void begin() {
    m_server->on("/parameter", [this](){ Handle_PostConfig(); });
    m_server->onNotFound([this](){ Handle_NotFound(); });
    m_server->begin();
  }

  const Parameter& GetParameter() {
    return m_parameter;
  }

  void HandleClient() {
    m_server->handleClient();
  }

  void SetFunc_OnConfigUpdated(std::function<void()> f) {
    m_onConfigUpdated = f;
  }
private:
  WebServer* m_server;
  StaticJsonDocument<1024> m_jsonDoc;
  semaphore_t* m_semaphore;
  Parameter m_parameter;
  std::function<void(void)> m_onConfigUpdated;

  void SetHeaders() {
    m_server->sendHeader("Access-Control-Allow-Origin",
                         "*");
    m_server->sendHeader("Access-Control-Allow-Credentials",
                         "true");
    m_server->sendHeader("Access-Control-Allow-Methods",
                         "POST");
  }

  void Handle_PostConfig() {
    SetHeaders();

    if(m_server->method() != HTTP_POST) {
      m_server->send(405, "text/plain", "Method not allowed");
      return;
    }

    if(m_server->hasArg("plain") == false) {
      m_server->send(400, "text/plain", "Bad request");
      return;
    }

    String body = m_server->arg("plain");
    DeserializationError jsonErr
      = deserializeJson(m_jsonDoc, body);
    if(jsonErr) {
      m_server->send(400, "text/plain",
                     String("Bad request:") + jsonErr.c_str());
      return;
    }

    bool acquired = sem_acquire_timeout_ms(m_semaphore, TIMEOUT_MS_SEMAPHORE);
    if(acquired == false) {
      m_server->send(503, "text/plain", "Service Unavailable");
      return;
    }

    m_parameter.antCount
      = static_cast<uint8_t>(m_jsonDoc["ants"]["count"]
                             .as<uint32_t>());
    JsonArray antsParams
      = m_jsonDoc["ants"]["params"].as<JsonArray>();
    int antNo = 0;
    for(JsonVariant jsonvar : antsParams) {
      uint32_t antsParam = jsonvar.as<uint32_t>();
      m_parameter.initialAnts[antNo].direction =
        static_cast<Direction>(antsParam & 0x3);
      m_parameter.initialAnts[antNo].position.x =
        (antsParam >> 2) & 0xFF;
      m_parameter.initialAnts[antNo].position.y =
        (antsParam >> 10) & 0x1FF;
      antNo++;
    }
    m_parameter.ruleLength = m_jsonDoc["rule"]["length"];
    uint32_t rule = m_jsonDoc["rule"]["rule"].as<uint32_t>();
    for(int i = 0; i < m_parameter.ruleLength; i++) {
      m_parameter.rule[i] =
        static_cast<Behaviour>((rule >> (2*i)) & 0x3);
    }

    sem_release(m_semaphore);

    m_onConfigUpdated();

    m_server->send(200, "text/plain", "OK");
  }

  void Handle_NotFound() {
    SetHeaders();
    m_server->send(404, "text/plain", "Not Found");
  }
};

#endif
