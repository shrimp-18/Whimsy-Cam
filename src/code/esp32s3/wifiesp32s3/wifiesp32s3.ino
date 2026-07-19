#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "WhimsyCam";
const char* password = "12345678";

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
}

void loop()
{
  HTTPClient http;

  http.begin("http://192.168.4.1/ping");

  int code = http.GET();

  if (code > 0)
  {
    Serial.print("HTTP Code: ");
    Serial.println(code);

    Serial.print("Response: ");
    Serial.println(http.getString());
  }
  else
  {
    Serial.println("Request failed");
  }

  http.end();

  delay(2000);
}