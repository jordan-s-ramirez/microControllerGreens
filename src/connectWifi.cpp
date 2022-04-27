#include "connectWifi.h"
#include "soc/sens_reg.h"
#include "DataStructures.h"

// Server
AsyncWebServer server(80); // This creates a web server, required in order to host a page for connected devices
DNSServer dnsServer;       // This creates a DNS server, required for the captive portal

// booleans
bool gotInfo = false;  // Check if we got info
bool tryAgain = false; // For User to try again

// Databse
String serverName = "https://microcontrollergreens.live/ESPSendAndRecieve.php";
// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "tPmAT5Ab3j7F9";

void wifiSetup() {
  createSAP();
  createWebServer();
  wifiSetupLoop();
}

void connectToWifi(const char* ssid, const char* password)
{
  // Check to see if we got info
  Serial.println("Trying Connection with:");
  Serial.println(ssid);
  Serial.println(password);

  WiFi.begin(ssid, password);

  // Try WiFi
  // Connect to WiFi
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Trying to Connect... ");
  }
  // Check WiFi
  Serial.print("Connected to WiFi!\n");
  Serial.println(WiFi.localIP());
}

void webRequest(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("text/html");

  // Header
  response->printf("<!doctype html><html lang='en'><head><meta charset='utf-8'>");
  response->printf("<meta name='viewport' content='width=device-width, initial-scale=1'>");

  if(gotInfo) {
    response->printf("<h2>Connected! Please visit microcontrollergreens.live</h2>");
  }
  else if(tryAgain) {
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
  // Enabling WiFi as soft access point and station
  WiFi.mode(WIFI_AP_STA);

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

    // Connect to WiFi
    connectToWifi(ssid.c_str(), password.c_str());

    if(WiFi.status() != WL_CONNECTED) {
      // Turn on server response
      Serial.println("Set Try again to True");
      tryAgain = true;

      // Update responses
      webRequest(request);
    }
    else {
      gotInfo = true;
      webRequest(request);
      // Turn off webserver
      Serial.println("Turn off DNS");
      dnsServer.stop();
      delay(10);
      Serial.println("Turn off Server");
      server.end();
      delay(10);
      Serial.println("Turn off SAP");
      WiFi.softAPdisconnect(false); // false to not turn off Wifi.mode()
      delay(10);
    }
  });
}

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

Preferences sendAndGetData(Measurements measurements) {
  Preferences preferences;
  // WiFiClient client;
  HTTPClient http;

  int breakBeam = measurements.breakBeam;
  int moisture = measurements.water;
  int light = measurements.light;

  // Your Domain name with URL path or IP address with path
  // http.begin(client, serverName);
  if(http.begin(serverName.c_str())) {
    Serial.println("Connected to database");
  }
    
  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // http.addHeader("Content-Type", "text/plain"); 
    
  // Prepare your HTTP POST request data
  String httpRequestData = "&api_key=" + apiKeyValue + "&breakBeam=" + breakBeam + "&moisture=" + moisture + "&light=" + light;

  // Send HTTP POST request
  int httpResponseCode = http.POST(httpRequestData);
  Serial.println("Data was sent");
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    // Serial.println(httpResponseCode);
    String infoString = http.getString();
    // Serial.println(infoString);

    // Parse info
    if(infoString.length() > 1) {
      int lightLevel = (int)infoString[0] - '0';
      int waterLevel = (int)infoString[1] - '0';
      preferences.lightLevel = (LightLevel)lightLevel;
      preferences.waterLevel = (WaterLevel)waterLevel;
      Serial.println("Data was recieved"); 
      Serial.println(infoString);
    } 
    else {
      preferences.lightLevel = (LightLevel)2;
      preferences.waterLevel = (WaterLevel)2;
      Serial.println("Data was NOT recieved"); 
    }
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  delay(10000);

  return preferences;
}

Preferences wifiLoop(Measurements measurements) {
  // send sensor values to database
  Preferences preferences = sendAndGetData(measurements);

  // get preferences from database
  // disconnect from wifi
  // restore wifi register
  // return preferences
  delay(100);

  return preferences;
}

void wifiSetupLoop() {
  while(WiFi.status() != WL_CONNECTED) {
      dnsServer.processNextRequest();
      delay(10);

      // Fixes Watch Dog Timer Issue
      TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
      TIMERG0.wdt_feed=1;
      TIMERG0.wdt_wprotect=0;
  }
}