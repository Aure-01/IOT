#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Definición de las credenciales de la red Wi-Fi a la que se conectará el dispositivo
const char *ssid = "A_Escolares_Jefatura";
const char *password = "itcolima6";

// Dirección URL del servidor web que recibirá los datos de temperatura
String serverName = "http://3e1b-187-190-35-202.ngrok-free.app";

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void postTemperature(float temperature) {
  DynamicJsonDocument json_chido(1024);
  json_chido["temperature"] = temperature;

  String json_str;
  serializeJson(json_chido, json_str);

  HTTPClient http;
  http.begin(serverName + "/temperature");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(json_str);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  dht.begin(); // Inicializa el sensor DHT11
}

void loop() {
  // Lectura de temperatura del sensor DHT11
  float temperature = dht.readTemperature();

  if (!isnan(temperature) && temperature >= -40 && temperature <= 80) {
    Serial.print("Temperatura: ");
    Serial.println(temperature);
    // Envía la temperatura al servidor
    postTemperature(temperature);
  } else {
    Serial.println("Error al leer la temperatura del sensor DHT11");
  }

  delay(5000); // Espera 5 segundos antes de realizar otra lectura
}
