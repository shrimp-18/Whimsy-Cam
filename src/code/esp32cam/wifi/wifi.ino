#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "WhimsyCam";
const char* password = "12345678";

WebServer server(80);

void handlePing()
{
  server.send(200, "text/plain", "PONG!");
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/ping", handlePing);

  server.begin();

  Serial.println("Server started");
}

void loop()
{
  server.handleClient();
}