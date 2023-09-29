#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *ssid = "LAPTOP-PEDROU";
const char *password = "12345678";

String serverName = "http://192.168.137.1:7800/";

const int buttonD4 = 4; // Pin D4
const int buttonD5 = 5; // Pin D5

void post_data(String action, int quantity)
{
  // Objeto JSON
  DynamicJsonDocument json_chido(1024);
  json_chido["action"] = action;
  json_chido["quantity"] = quantity;

  // Cadena JSON para enviar
  String json_str;
  serializeJson(json_chido, json_str);

  // Enviar POST
  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(json_str);

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(buttonD4, INPUT_PULLUP);
  pinMode(buttonD5, INPUT_PULLUP);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  if (digitalRead(buttonD4) == LOW)
  {
    // Button D4 is pressed, send HTTP POST with action "desc" and quantity 1
    post_data("desc", 1);
    delay(1000); // Avoid multiple presses in a short time
  }
  else if (digitalRead(buttonD5) == LOW)
  {
    // Button D5 is pressed, send HTTP POST with action "asc" and quantity 1
    post_data("asc", 1);
    delay(1000); // Avoid multiple presses in a short time
  }

  if (Serial.available())
  {
    String data = Serial.readStringUntil('\n');
    if (data == "+")
    {
      post_data("asc", 1);
    }
    else if (data == "-")
    {
      post_data("desc", 1);
    }
    else
    {
      Serial.println("Invalid command");
    }
  }
}