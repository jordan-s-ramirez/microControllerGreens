#pragma once

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <WiFi.h> // Wifi
#include <EEPROM.h> // Save to flash
#include <HTTPClient.h>

struct Settings
{
  char ssid[30];
  char password[30];
}; 

void connectToWifi();
void webRequest(AsyncWebServerRequest *request);
void createSAP();
void onGet();
void onNotFound();
void webServerSetup();
void createWebServer();
void sendData();
Settings getWifiInfo();
void setUserInfo(String ssid, String password);
void testWifiConnection();
void wifiLoop();

