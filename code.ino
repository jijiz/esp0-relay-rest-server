#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
 
const char* SSID = "Obi-free";
const char* password = "2BD";

// Setup GPIO0
int pinGPIO0 = 0; // Relay pin
int relayStatus = 0; //0=off,1=on,2=dimmed
 
ESP8266WebServer server(80);

void relayOn() {
    server.send(200, "text/json", "{\"name\": \"On\"}");
	digitalWrite(pinGPIO0, LOW);
    relayStatus = 1;
}

void relayOff() {
    server.send(200, "text/json", "{\"name\": \"Off\"}");
	digitalWrite(pinGPIO0, HIGH);
    relayStatus = 0;
}
 
// Define routing
void restServerRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("Les fraisiers vous souhaitent la bienvenue"));
    });
    server.on(F("/ON"), HTTP_GET, relayOn);
	server.on(F("/OFF"), HTTP_GET, relayOff);
}
 
// Manage not found URL
void handleNotFound() {
  String message = "Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
 
void setup(void) {
	
	
  Serial.begin(115200);
  pinMode(pinGPIO0, OUTPUT);
  digitalWrite(pinGPIO0, HIGH);
  relayStatus = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.hostname("Fraisiers");
  WiFi.begin(SSID, password);
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane fraisiers.local
  if (MDNS.begin("fraisiers")) {
    Serial.println("MDNS responder started");
  }
 
  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void) {
  server.handleClient();
}