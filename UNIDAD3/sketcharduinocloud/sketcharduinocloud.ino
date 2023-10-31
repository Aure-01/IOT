#include "thingProperties.h"
#include <DHT.h>

// Pin del sensor DHT11
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

bool lastButtonAumentarState = LOW;  // Estado anterior del botón Aumentar
bool lastButtonDisminuirState = LOW; // Estado anterior del botón Disminuir

void setup() {
  Serial.begin(9600);
  delay(1500);

  initProperties();

  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Configuración del pin del LED (D21 en un ESP32)
  pinMode(21, OUTPUT);

  // Configuración de los pines de los botones del contador
  pinMode(4, INPUT);  // Botón Aumentar (D4)
  pinMode(5, INPUT);  // Botón Disminuir (D5)

  // Inicialización del sensor DHT11
  dht.begin();
}

void loop() {
  ArduinoCloud.update();

  // Leer el estado actual de los botones
  bool buttonAumentarState = digitalRead(4);
  bool buttonDisminuirState = digitalRead(5);
  
  // Actualizar el estado del LED (D21)
  digitalWrite(21, led);

  // Leer la temperatura y humedad del sensor DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (!isnan(temperature) && temperature >= -40 && temperature <= 80) {
    temperatura = temperature;
    humedad = humidity;
  }

  // Incrementar el contador si se presiona el botón de aumento (D4)
  if (buttonAumentarState == HIGH && lastButtonAumentarState == LOW) {
    contador += 1;
  }

  // Decrementar el contador si se presiona el botón de disminuir (D5)
  if (buttonDisminuirState == HIGH && lastButtonDisminuirState == LOW) {
    contador -= 1;
  }

  // Actualizar el estado anterior de los botones
  lastButtonAumentarState = buttonAumentarState;
  lastButtonDisminuirState = buttonDisminuirState;
}

void onLedChange()  {
  // Add your code here to act upon Led change
}

void onContadorChange()  {
  // Add your code here to act upon Contador change
}

void onTemperaturaChange()  {
  // Add your code here to act upon Temperatura change
}

void onHumedadChange()  {
  // Add your code here to act upon Humedad change
}
