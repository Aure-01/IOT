#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// Definición de las credenciales de la red Wi-Fi a la que se conectará el dispositivo
const char *ssid = "W_Aula_WB11";
const char *password = "itcolima6";

// Dirección URL del servidor web
String serverName = "http://565f-187-190-35-202.ngrok-free.app"; // Reemplaza con la dirección de tu servidor

// Pin del LED que se controlará
const int ledPin = 21;
bool ledStatus = false; // Estado inicial del LED

// Pin de los botones del contador
const int botonAumentarPin = 4;
const int botonDisminuirPin = 5;
int contador = 0;

// Pin del sensor DHT11
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Función para actualizar el estado del LED
void updateLED() {
  digitalWrite(ledPin, ledStatus ? HIGH : LOW);
}

// Función para realizar una solicitud HTTP al servidor y obtener el estado del LED
void getLEDStatus() {
  HTTPClient http;
  http.begin(serverName + "/led"); // Realiza una solicitud GET a la ruta "/led" del servidor

  int httpResponseCode = http.GET();

  if (httpResponseCode == HTTP_CODE_OK) {
    String payload = http.getString(); // Lee la respuesta del servidor

    // Analiza la respuesta JSON del servidor
    DynamicJsonDocument jsonLed(1024);
    deserializeJson(jsonLed, payload);

    // Verifica si el campo "status" está presente en la respuesta JSON
    if (jsonLed.containsKey("status")) {
      bool serverLedStatus = jsonLed["status"];
      ledStatus = serverLedStatus;
      updateLED(); // Actualiza el estado del LED según la respuesta del servidor
    }
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

// Función para enviar datos al servidor del contador
void post_data(String action, int quantity) {
  DynamicJsonDocument json_chido(1024);
  json_chido["action"] = action;
  json_chido["quantity"] = quantity;

  String json_str;
  serializeJson(json_chido, json_str);

  HTTPClient http;
  http.begin(serverName + "/counter");
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

// Función para enviar datos al servidor de temperatura y humedad
void postTemperatureAndHumidity(float temperature, float humidity) {
  DynamicJsonDocument jsonData(1024);
  jsonData["temperature"] = temperature;
  jsonData["humidity"] = humidity;

  String jsonStr;
  serializeJson(jsonData, jsonStr);

  HTTPClient http;
  http.begin(serverName + "/temperature"); // Cambia la ruta al servidor para temperatura
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(jsonStr);

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

  // Conexión a la red Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Configuración del pin del LED
  pinMode(ledPin, OUTPUT);
  updateLED(); // Configura el estado inicial del LED

  // Configuración de los pines de los botones del contador
  pinMode(botonAumentarPin, INPUT);
  pinMode(botonDisminuirPin, INPUT);

  // Inicialización del sensor DHT11
  dht.begin();
}

void loop() {
  // Control del LED a través de solicitudes HTTP
  getLEDStatus(); // Obtiene el estado del LED desde el servidor web

  // Lectura de temperatura y humedad del sensor DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (!isnan(temperature) && temperature >= -40 && temperature <= 80) {
    Serial.print("Temperatura: ");
    Serial.println(temperature);
    // Envía la temperatura y humedad al servidor
    postTemperatureAndHumidity(temperature, humidity);
  } else {
    Serial.println("Error al leer la temperatura del sensor DHT11");
  }

  // Lee el estado de los botones para aumentar y disminuir el contador
  int estadoBotonAumentar = digitalRead(botonAumentarPin);
  int estadoBotonDisminuir = digitalRead(botonDisminuirPin);

  // Incrementa el contador si se presiona el botón de aumento
  if (estadoBotonAumentar == HIGH) {
    contador++;
    // Envía la acción "asc" (aumentar) y la cantidad 1 al servidor
    post_data("asc", 1);
  }
  // Decrementa el contador si se presiona el botón de disminución
  else if (estadoBotonDisminuir == HIGH) {
    contador--;
    // Envía la acción "desc" (disminuir) y la cantidad 1 al servidor
    post_data("desc", 1);
  }

  // Imprime el valor del contador en el puerto serie
  Serial.print("Contador: ");
  Serial.println(contador);

  delay(1000); // Espera 1 segundo antes de realizar otra lectura
}
