#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Definición de las credenciales de la red Wi-Fi a la que se conectará el dispositivo
const char *ssid = "A_Escolares_Jefatura";
const char *password = "itcolima6";

// Dirección URL del servidor web que proporciona el estado del LED
String serverName = "http://7a5e-187-190-35-202.ngrok-free.app";

// Pin del LED que se controlará
const int ledPin = 21;

bool ledStatus = false; // Estado inicial del LED

// Función para actualizar el estado del LED en función de la variable ledStatus
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
    DynamicJsonDocument json_chido(1024);
    deserializeJson(json_chido, payload);

    // Verifica si el campo "status" está presente en la respuesta JSON
    if (json_chido.containsKey("status")) {
      bool serverLedStatus = json_chido["status"];
      ledStatus = serverLedStatus;
      updateLED(); // Actualiza el estado del LED según la respuesta del servidor
    }
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

  pinMode(ledPin, OUTPUT);
  updateLED(); // Configura el estado inicial del LED
}

void loop() {
  // Realiza cualquier tarea relacionada con los LEDs aquí
  // Por ejemplo, cambiar el estado del LED según alguna condición

  // Control del LED a través de solicitudes HTTP
  getLEDStatus(); // Obtiene el estado del LED desde el servidor web
}
