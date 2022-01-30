#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

/// Web Server Stuff
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "DNSServer.h"

// Server
AsyncWebServer server(80); // This creates a web server, required in order to host a page for connected devices
DNSServer dnsServer;       // This creates a DNS server, required for the captive portal
WiFiUDP udp;

struct settings
{
  char ssid[30];
  char password[30];
} user_wifi = {};

// Got info?
bool gotInfo = false;

void connectToWifi()
{
  // Make WiFi as a Station
  WiFi.mode(WIFI_STA);
  // Check to see if we got info
  if (user_wifi.ssid[0] != 0)
  {
    Serial.println("Trying Connection with:");
    Serial.println(user_wifi.ssid);
    Serial.println(user_wifi.password);

    WiFi.begin(user_wifi.ssid, user_wifi.password);

    // Try WiFi
    byte tries = 0;
    // Connect to WiFi
    while ((WiFi.status() != WL_CONNECTED ) && tries < 5)
    {
      vTaskDelay(500 / portTICK_PERIOD_MS);
      Serial.println("Trying to Connect... ");
      tries++;
    }
  }

  // Check WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Failed to Connect to WiFi!");
    gotInfo = false;
  }
  else
  {
    Serial.print("Connected to WiFi!\n");
    Serial.println(WiFi.localIP());
    gotInfo = true;
  }
}

void webRequest(AsyncWebServerRequest *request)
{
  request->send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>MicroController Greens Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}</style> </head> <body><main class='form-signin'> <form action='/get' method='post'> <h1 class=''>MicroController Greens Wifi Setup</h1><br/><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'></div><br/><br/><button type='submit'>Save</button><p style='text-align: right'></p></form></main> </body></html>");
}

void createSAP() {
  // Enabling WiFi as soft access point
  WiFi.mode(WIFI_MODE_AP);

  Serial.println("Setting Soft Access Point: Setup");
  Serial.println(WiFi.softAP("MicroController Greens", NULL) ? "Ready" : "Failed!");

  Serial.print("SAP IP Address: ");
  Serial.println(WiFi.softAPIP());

}


void onGet(AsyncWebServerRequest *request)
{
  String ssid;
  String password;

  // CHECK Which Type
  // if(request->hasParam("ssid")) {
  //   Serial.println("No True");
  // }
  // if(request->hasParam("ssid", true)) {
  //   Serial.println("One True");
  // }
  // if(request->hasParam("ssid", true, true)) {
  //   Serial.println("Two True");
  // }

  // Checking for inputs
  if (request->hasParam("ssid", true)) {
    ssid = request->getParam("ssid", true)->value();
    Serial.println("SSID: " + ssid);
  }
  if (request->hasParam("password"), true) {
    password = request->getParam("password", true)->value();
    Serial.println("Password: " + password);
  }

  // Check for length less than 30
  if ((password.length() <= 30) || (ssid.length() <= 30))
  {
    // Update webpage
    request->send(200, "text/html", "<h1>WiFi SSID: " + ssid + ", and Password: " + password + " Saved! Trying connection...</h1>");

    // Save to FLASH
    strncpy(user_wifi.ssid, ssid.c_str(), sizeof(user_wifi.ssid));
    strncpy(user_wifi.password, password.c_str(), sizeof(user_wifi.password));
    user_wifi.ssid[ssid.length()] = user_wifi.password[password.length()] = '\0';

    // Connect to Wifi
    connectToWifi();

    if (WiFi.status() == WL_CONNECTED) {
      //request->send(200, "text/html", "<h1>MicroController Greens has Connected to WiFi</h1>");

      // Save to FLASH
      EEPROM.put(0, user_wifi);
      EEPROM.commit();

      Serial.println("SSID and Password saved to FLASH");
      Serial.println("SSID: " + ssid);
      Serial.println("Password: " + password);

      // Shut Down DNS Server
      Serial.println("Server SHUTDOWN");
      server.end();
      dnsServer.stop();
      WiFi.softAPdisconnect(); 
    }
    else {
      // Update User
      // request->send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><h1>Incorrect WiFi SSID or password, please try again.</h1><title>MicroController Greens Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}</style> </head> <body><main class='form-signin'> <form action='/get' method='post'> <h1 class=''>MicroController Greens Wifi Setup</h1><br/><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'></div><br/><br/><button type='submit'>Save</button><p style='text-align: right'></p></form></main> </body></html>");
      
      // Clean User Wifi and Password
      Serial.print("Cleaning");
      for(int i = 0; i < 30; i++) {
        Serial.print(".");
        user_wifi.password[i] = 0;
        user_wifi.ssid[i] = 0;
      }
      // createSAP();
      //createWebServer();
    }
  }
  else
  {
    Serial.println("SSID or Password Too long");
    request->send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><h1>Wifi SSID or Password too long, please try again.</h1><title>MicroController Greens Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}</style> </head> <body><main class='form-signin'> <form action='/get' method='post'> <h1 class=''>MicroController Greens Wifi Setup</h1><br/><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'></div><br/><br/><button type='submit'>Save</button><p style='text-align: right'></p></form></main> </body></html>");
  
  }

}


void webServerSetup(){ 
  // //This is a super simple page that will be served up any time the root location is requested.  Get here intentionally by typing in the IP address.
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.print("server.web triggered: ");
    Serial.println(request->url());
    webRequest(request);
  });


  // For Android
  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.print("server.android triggered: ");
    Serial.println(request->url());
    webRequest(request);
  });

  //onNotFound acts as a catch-all for any request not defined above
  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.print("server.notfound triggered: ");
    Serial.println(request->url()); 

    if(strcmp("/get", (request->url()).c_str()) == 0) {
      onGet(request);
    }
    else {
      webRequest(request);
    }
  });

  server.begin();                         //Starts the server process
  Serial.println("Web server started");
}

void createWebServer() {
  // DNS Stuff
  dnsServer.start(53, "*", WiFi.softAPIP());  //This starts the DNS server.  The "*" sends any request for port 53 straight to the IP address of the device
  webServerSetup();                       //Configures the behavior of the web server
  Serial.println("Setup complete");
}

void setup() {
  Serial.begin(9600);
  // Allocate Storage For WiFi

  EEPROM.begin(sizeof(struct settings) );
  EEPROM.put(0, user_wifi); // DELETES PREVIOSU SAVED WIFI SETTINGS
  EEPROM.get(0, user_wifi);
  
  // Try WiFi Connection, will create SAP if fails
  connectToWifi();

  // If Not connected to WiFi
  if(WiFi.status() != WL_CONNECTED) {
    // Create SAP portal Again
    createSAP();
    createWebServer();
  }
}

void loop() {
  if(!gotInfo) {
    //server.handleClient();
    dnsServer.processNextRequest(); 
  }
  else if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting");
    connectToWifi();
  }
  else {
    Serial.println("Connected!");
  }

}
