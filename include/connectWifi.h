#pragma once

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <WiFi.h> // Wifi
#include <HTTPClient.h>
#include "DataStructures.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

void wifiSetup();
void connectToWifi(const char* ssid, const char* password);
void webRequest(AsyncWebServerRequest *request);
void createSAP();
void onGet();
void onNotFound();
void webServerSetup();
void createWebServer();
Preferences sendAndGetData(Measurements measurements);
Preferences wifiLoop(Measurements measurements);
void wifiSetupLoop();
