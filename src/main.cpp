#include <Arduino.h>
#include <WiFi.h>
// #include <WebServer.h>
#include <EEPROM.h>

#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "DNSServer.h"

// Server
// WebServer server(80);
AsyncWebServer server(80); //This creates a web server, required in order to host a page for connected devices
DNSServer dnsServer;     //This creates a DNS server, required for the captive portal

struct settings {
  char ssid[30];
  char password[30];
} user_wifi = {};

// Got info?
bool gotInfo = false;

// void handlePortal() {
//   if (server.method() == HTTP_POST) {
//     // Saving WiFi Data to the flash
//     strncpy(user_wifi.ssid,     server.arg("ssid").c_str(),     sizeof(user_wifi.ssid) );
//     strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password) );
//     user_wifi.ssid[server.arg("ssid").length()] = user_wifi.password[server.arg("password").length()] = '\0';
//     EEPROM.put(0, user_wifi);
//     EEPROM.commit();

//     server.send(200,   "text/html",  "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>Wifi Setup</h1> <br/> <p>Your settings have been saved successfully!<br />Please restart the device.</p></main></body></html>" );
//     gotInfo = true;
//   } else {

//     server.send(200,   "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}</style> </head> <body><main class='form-signin'> <form action='/' method='post'> <h1 class=''>MicroController Greens Wifi Setup</h1><br/><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'></div><br/><br/><button type='submit'>Save</button><p style='text-align: right'></p></form></main> </body></html>" );
//   }
// }

void webServerSetup(){
  
  //This is a super simple page that will be served up any time the root location is requested.  Get here intentionally by typing in the IP address.
  server.on(("/"), HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}</style> </head> <body><main class='form-signin'> <form action='/get' method='post'> <h1 class=''>MicroController Greens Wifi Setup</h1><br/><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'></div><br/><br/><button type='submit'>Save</button><p style='text-align: right'></p></form></main> </body></html>" );
  });


  //This is an example of triggering for a known location.  This one seems to be common for android devices
  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}</style> </head> <body><main class='form-signin'> <form action='/get' method='post'> <h1 class=''>MicroController Greens Wifi Setup</h1><br/><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'></div><br/><br/><button type='submit'>Save</button><p style='text-align: right'></p></form></main> </body></html>" );

  });

  //This is an example of a redirect type response.  onNotFound acts as a catch-all for any request not defined above
  server.onNotFound([](AsyncWebServerRequest *request){
    request->redirect(("/" + WiFi.softAPIP().toString()));
    Serial.print("server.notfound triggered: ");
    Serial.println(request->url());       //This gives some insight into whatever was being requested
  });

  // Get Info???
  // server.on("/get")

  server.begin();                         //Starts the server process
  Serial.println("Web server started");
}

void createSAP() {
  // Enabling WiFi as soft access point
  WiFi.mode(WIFI_MODE_AP);

  // Serial.print("Setting Soft Access Point: Configuration");
  // Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.println("Setting Soft Access Point: Setup");
  Serial.println(WiFi.softAP("MicroController Greens", NULL) ? "Ready" : "Failed!");

  Serial.print("SAP IP Address: ");
  Serial.println(WiFi.softAPIP());


  // DNS Stuff
  dnsServer.start(53, "*", WiFi.softAPIP());  //This starts the DNS server.  The "*" sends any request for port 53 straight to the IP address of the device
  webServerSetup();                       //Configures the behavior of the web server
  Serial.println("Setup complete");
 
}

void connectToWifi() {
  // Make WiFi as a Station
  WiFi.mode(WIFI_STA);
  // Check to see if we got info
  if(gotInfo) {
    // Connect to WiFi 
    WiFi.begin(user_wifi.ssid, user_wifi.password);

    // Try WiFi
    byte tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Trying to Connect... ");
      if (tries++ > 30) {
        break;
      }
    }
  }

  // Check WiFi  
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed!");
    // Create SAP portal Again
    createSAP();
    gotInfo = false;
  }
  else {
    Serial.print("Connected!\n");
    Serial.println(WiFi.localIP());
    gotInfo = true;
  }
}

void checkInput() {
  
}

void setup() {
  Serial.begin(9600);
  // Allocate Storage For WiFi
  // THIS IS A WILL DELETE THE SAVED INPUT DELETE THE LINE BEWLOW
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }

  EEPROM.begin(sizeof(struct settings) );
  EEPROM.get( 0, user_wifi );
  
  // Try WiFi Connection, will create SAP if fails
  connectToWifi();

  // Setup Portal OLD
  // server.on("/",  handlePortal);
  // server.begin();

}

void loop() {
  if(!gotInfo) {
    //Serial.println("Getting INFO");
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
