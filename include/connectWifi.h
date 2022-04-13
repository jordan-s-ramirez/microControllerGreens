#pragma once

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <WiFi.h> // Wifi
#include <EEPROM.h> // Save to flash
#include <HTTPClient.h>
#include "DataStructures.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

struct Settings
{
  char ssid[30];
  char password[30];
}; 
static Settings user_wifi = {0};

void connectToWifi();
void webRequest(AsyncWebServerRequest *request);
void createSAP();
void onGet();
void onNotFound();
void webServerSetup();
void createWebServer();
Preferences sendAndGetData(int breakBeam, int moisture, int light);
Settings getWifiInfo();
Preferences wifiLoop(Measurements measurements);
void wifiSetupLoop();
