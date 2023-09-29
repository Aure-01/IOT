#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Definición de las credenciales de la red Wi-Fi a la que se conectará el dispositivo
const char *ssid = "A_Escolares_Jefatura";
const char *password = "itcolima6";

// Dirección URL del servidor web que proporciona el estado del LED
String serverName = "http://7a5e-187-190-35-202.ngrok-free.app";

int contador = 0;  // Inicializa el contador en 0
const int botonAumentarPin = 4;
const int botonDisminuirPin = 5;

// Función para enviar datos al servidor, como la acción (asc o desc) y la cantidad
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

  pinMode(botonAumentarPin, INPUT);
  pinMode(botonDisminuirPin, INPUT);
}

void loop() {
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
