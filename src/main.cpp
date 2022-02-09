#include <Arduino.h>
#include <WiFi.h> // Wifi
#include <EEPROM.h> // Save to flash

/// Web Server Stuff
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

// Databse
#include <HTTPClient.h>

// Server
AsyncWebServer server(80); // This creates a web server, required in order to host a page for connected devices
DNSServer dnsServer;       // This creates a DNS server, required for the captive portal

// User WiFi Settings
struct settings
{
  char ssid[30];
  char password[30];
} user_wifi = {};

// booleans
bool gotInfo = false;  // Check if we got info
bool tryAgain = false; // For User to try again
bool proResponse = true; // Stop processing response
bool updateOnNotFound = false; // Update onNotFound

// Databse
const char* serverName = "https://jordanramirez.com/post-esp-data.php";
// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "tPmAT5Ab3j7F9";

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
    while ((WiFi.status() != WL_CONNECTED ) && tries < 10)
    {
      vTaskDelay(400 / portTICK_PERIOD_MS);
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

void webRequest(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("text/html");

  // Header
  response->printf("<!doctype html><html lang='en'><head><meta charset='utf-8'>");
  response->printf("<meta name='viewport' content='width=device-width, initial-scale=1'>");

  if(tryAgain) {
    response->printf("<h2>Incorrect user WiFi data, please try again!</h2>");
  }

  response->printf("<body style=\"background: #e0f9e7\">");
  // Title
  response->printf("<h1 style=\"text-align:center; background: #aff1c1\">MicroController Greens Wifi Setup</h1>");
  // Form
  response->printf("<form style=\"text-align: center\" action=\"/get\">");
  response->printf("<label>SSID</label><input type=\"text\" name=\"ssid\">");
  response->printf("</br><label>Password</label><input type=\"password\" name=\"password\">");
  response->printf("</br><input type=\"submit\" value=\"Submit\">");
  response->printf("</form>");
  response->printf("</main></body></html>");

  request->send(response);
}

void createSAP() {
  // Enabling WiFi as soft access point
  WiFi.mode(WIFI_MODE_AP);

  Serial.println("Setting Soft Access Point: Setup");
  Serial.println(WiFi.softAP("MicroController Greens", NULL) ? "Ready" : "Failed!");

  Serial.print("SAP IP Address: ");
  Serial.println(WiFi.softAPIP());

}

void onGet() {
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("GET Request in progress");
    // Vars
    String ssid;
    String password;

    // Checking for inputs
    if (request->hasParam("ssid")) {
      ssid = request->getParam("ssid")->value();
      Serial.println("SSID: " + ssid);
    }
    if (request->hasParam("password")) {
      password = request->getParam("password")->value();
      Serial.println("Password: " + password);
    }

    // Send Update to User
    request->send(200, "text/html", "<h1>WiFi SSID: " + ssid + ", and Password: " + password + " Saved! Trying connection... If not connected please try again, and rejoin MicroController Greens WiFi </h1>");


    // Transfer User data to settings
    strncpy(user_wifi.ssid, ssid.c_str(), sizeof(user_wifi.ssid));
    strncpy(user_wifi.password, password.c_str(), sizeof(user_wifi.password));
    user_wifi.ssid[ssid.length()] = user_wifi.password[password.length()] = '\0'; 

    // Turn off responses
    proResponse = false;
    server.onNotFound(NULL);

    // Connect to WiFi
    connectToWifi();

    if(WiFi.status() != WL_CONNECTED) {
      // Turn on server response
      Serial.println("Set Try again to True");
      tryAgain = true;

      // Clean user_wifi
      Serial.print("Cleaning old WiFi Data");
      for(int i = 0; i < 30; i++) {
        Serial.print(".");
        user_wifi.password[i] = 0;
        user_wifi.ssid[i] = 0;
      }
      Serial.print("\n");

      // Update responses
      updateOnNotFound = true;
      vTaskDelay(1000 / portTICK_PERIOD_MS); 
      proResponse = true;
      webRequest(request);
    }
    else {
      // Get Info
      gotInfo = true;
      // Turn off webserver
      Serial.println("Turn off DNS");
      dnsServer.stop();
      Serial.println("Turn off Server");
      server.end();
      Serial.println("Turn off SAP");
      WiFi.softAPdisconnect();
      // Save data
      EEPROM.put(0, user_wifi);

    }

  });
}
/*
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
    request->send(200, "text/html", "<h1>WiFi SSID: " + ssid + ", and Password: " + password + " Saved! Trying connection... If not connected please try again, and rejoin MicroController Greens WiFi </h1>");

    vTaskDelay(1000 / portTICK_PERIOD_MS); 
  
    // Save to WiFi Data
    strncpy(user_wifi.ssid, ssid.c_str(), sizeof(user_wifi.ssid));
    strncpy(user_wifi.password, password.c_str(), sizeof(user_wifi.password));
    user_wifi.ssid[ssid.length()] = user_wifi.password[password.length()] = '\0';    

    // Remove server handler
    server.onNotFound(NULL);
    Serial.println("Stop Processing Response: Stop");

    // Shut Down DNS Server
    // Serial.println("Server Shutting Down");
    // dnsServer.stop();
    // server.end();
    // WiFi.softAPdisconnect();

    // Turn off Processing response
    Serial.println("Processing set to false");
    proResponse = false;
    connectToWifi();
    
    if(WiFi.status() != WL_CONNECTED) {
      // Turn on server response
      Serial.println("Set Try again to True");
      tryAgain = true;

      // Clean user_wifi
      Serial.print("Cleaning old WiFi Data");
      for(int i = 0; i < 30; i++) {
        Serial.print(".");
        user_wifi.password[i] = 0;
        user_wifi.ssid[i] = 0;
      }
      Serial.print("\n");

      // Update responses
      updateOnNotFound = true;
      vTaskDelay(1000 / portTICK_PERIOD_MS); 
      proResponse = true;
      webRequest(request);
    }
  }
  else
  {
    Serial.println("SSID or Password Too long");
    request->send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><h1>Wifi SSID or Password too long, please try again.</h1><title>MicroController Greens Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}</style> </head> <body><main class='form-signin'> <form action='/get' method='post'> <h1 class=''>MicroController Greens Wifi Setup</h1><br/><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'></div><br/><br/><button type='submit'>Save</button><p style='text-align: right'></p></form></main> </body></html>");

  }

}
*/
void onNotFound() {
  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.print("server.notfound triggered: ");
    Serial.println(request->url()); 
    webRequest(request);
  });
}

void webServerSetup(){ 
  //onNotFound acts as a catch-all for any request
  onNotFound();
  //onGet
  onGet();

  // Start Server
  try {
    Serial.println("[Try] to start Web Server");
    server.begin(); //Starts the server process
  }
  catch (const std::exception&) {
    Serial.println("[Catch] start Web Server");
    server.begin();
  }
  Serial.println("Web server started");
}

void createWebServer() {
  // DNS Stuff
  boolean start = dnsServer.start(53, "*", WiFi.softAPIP());  //This starts the DNS server.  The "*" sends any request for port 53 straight to the IP address of the device
  if(!start) {
    Serial.println("DNS server did not start");
  }
  Serial.println("DNS server started!");
  webServerSetup();                       //Configures the behavior of the web server
  Serial.println("Setup complete");
}

void sendData() {
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
    
  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
  // Prepare your HTTP POST request data
  String httpRequestData = "&api_key=" + apiKeyValue + "&test=" + "STRINGSENT";

  // Send HTTP POST request
  int httpResponseCode = http.POST(httpRequestData);
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  Serial.println("Data was sent");
  delay(10000);
}

void setup() {
  Serial.begin(9600);
  // Allocate Storage For WiFi
  EEPROM.begin(sizeof(struct settings) );
  // EEPROM.put(0, user_wifi); // DELETES PREVIOSU SAVED WIFI SETTINGS
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
    if(proResponse) {
      // Handle WiFi
      dnsServer.processNextRequest();
      vTaskDelay(10);
    }
    if(updateOnNotFound) {
      Serial.println("Update onNotFound to take inputs");
      onNotFound();
      vTaskDelay(100);
      updateOnNotFound = false;
    }
  } 
  if(WiFi.status() != WL_CONNECTED) {
    connectToWifi();
  }
  else {
    Serial.println("Connected!");
    delay(1000);
    sendData();
  }

}
