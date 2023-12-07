#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#include <ESP32Servo.h>

// Declaración de funciones
void callback(char *topic, byte *payload, unsigned int length);
int obtenerDistancia(int triggerPin, int echoPin);
void controlarServos(Servo &servo, int triggerPin, int echoPin, int tiempoPluma);
void levantarPluma(Servo &servo, int tiempoPluma);
void bajarPluma(Servo &servo);

// WiFi
const char *ssid = "Cuarto de Aure_2.4G";   // Ingresa el nombre de tu WiFi
const char *password = "2WC456400946"; // Ingresa la contraseña de tu WiFi

// MQTT Broker
const char *mqttServer = "w3a4bbd9.ala.us-east-1.emqxsl.com";
const int mqttPort = 8883;
const char *mqttUser = "esp32";
const char *mqttPassword = "password";
const char *mqttTopic = "monitores/esp32";

const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure espClient;
PubSubClient client(espClient);

// Objetos para controlar los servos
Servo servo1;
Servo servo2;

// Pines para sensores ultrasónicos y servos
const int distanciaSensor1TriggerPin = 2;
const int distanciaSensor1EchoPin = 4;
const int distanciaSensor2TriggerPin = 19;
const int distanciaSensor2EchoPin = 21;
const int anguloServo1Pin = 5;
const int anguloServo2Pin = 22;

// Tiempo que la pluma debe estar arriba en milisegundos
int tiempoPluma = 1500;  // Puedes ajustar este valor según tus necesidades

// Variables para el manejo del tiempo
unsigned long tiempoAnterior = 0;
unsigned long intervaloEnvio = 500;  // Intervalo de envío en milisegundos

void setup() {
  // Inicializar los pines de los sensores y los servomotores
  pinMode(distanciaSensor1TriggerPin, OUTPUT);
  pinMode(distanciaSensor1EchoPin, INPUT);
  pinMode(distanciaSensor2TriggerPin, OUTPUT);
  pinMode(distanciaSensor2EchoPin, INPUT);

  servo1.attach(anguloServo1Pin);
  servo2.attach(anguloServo2Pin);

  // Inicializar la comunicación serial para la depuración
  Serial.begin(115200);

  // Conexión a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a la red WiFi");

  // Configuración del cliente MQTT con TLS/SSL
  espClient.setCACert(ca_cert);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("El cliente %s se conecta al servidor MQTT\n", client_id.c_str());

    if (client.connect(client_id.c_str(), mqttUser, mqttPassword)) {
      Serial.println("Conexión exitosa al servidor MQTT");
    } else {
      Serial.print("Error de conexión, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 2 segundos");
      delay(2000);
    }
  }

  // Publicación y suscripción
  client.publish(mqttTopic, "¡Hola, soy ESP32 con TLS/SSL ^^!");
  client.subscribe(mqttTopic);
}

void loop() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= intervaloEnvio) {
    controlarServos(servo1, distanciaSensor1TriggerPin, distanciaSensor1EchoPin, tiempoPluma);
    controlarServos(servo2, distanciaSensor2TriggerPin, distanciaSensor2EchoPin, tiempoPluma);

    DynamicJsonDocument doc(1024);
    doc["distancia1"] = obtenerDistancia(distanciaSensor1TriggerPin, distanciaSensor1EchoPin);
    doc["distancia2"] = obtenerDistancia(distanciaSensor2TriggerPin, distanciaSensor2EchoPin);

    int servo1Value = servo1.read();
    doc["servo1"] = constrain(servo1Value, 0, 90);

    int servo2Value = servo2.read();
    doc["servo2"] = constrain(servo2Value, 0, 90);

    String jsonString;
    serializeJson(doc, jsonString);

    client.publish(mqttTopic, jsonString.c_str());

    tiempoAnterior = tiempoActual;
  }

  client.loop();
}


void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Mensaje recibido en el tema: ");
    Serial.println(topic);

    // Convierte el payload en una cadena
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.print("Mensaje: ");
    Serial.println(message);

    // Procesa el mensaje JSON
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, message);

    // Verifica si el mensaje contiene la propiedad "action"
    if (doc.containsKey("action")) {
        String action = doc["action"];
        if (action.equals("activarServo1")) {
            // Activa físicamente el servo1
            levantarPluma(servo1, tiempoPluma);
        } else if (action.equals("activarServo2")) {
            // Activa físicamente el servo2
            levantarPluma(servo2, tiempoPluma);
        }
    } else {
        // Procesa otros datos del mensaje (distancia, servo1, servo2)
        if (doc.containsKey("distancia1") && doc.containsKey("distancia2") && doc.containsKey("servo1") && doc.containsKey("servo2")) {
            int distancia1 = doc["distancia1"];
            int distancia2 = doc["distancia2"];
            int servo1Value = doc["servo1"];
            int servo2Value = doc["servo2"];

            // Imprime la información recibida
            Serial.print("Distancia1: ");
            Serial.println(distancia1);
            Serial.print("Distancia2: ");
            Serial.println(distancia2);
            Serial.print("Servo1: ");
            Serial.println(servo1Value);
            Serial.print("Servo2: ");
            Serial.println(servo2Value);

            // Puedes hacer algo con la distancia y el valor del servo recibidos
            // Por ejemplo, actualizar variables globales, realizar acciones, etc.
        }
    }
}


int obtenerDistancia(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  return pulseIn(echoPin, HIGH) * 0.034 / 2;  // Calcular la distancia en centímetros
}

void controlarServos(Servo &servo, int triggerPin, int echoPin, int tiempoPluma) {
    int distancia = obtenerDistancia(triggerPin, echoPin);

    if (distancia < 15) {
        levantarPluma(servo, tiempoPluma);
    } else {
        bajarPluma(servo);
    }
}

void levantarPluma(Servo &servo, int tiempoPluma) {
  if (servo.read() != 90) {
    servo.write(90);
    delay(tiempoPluma);
  }
}

void bajarPluma(Servo &servo) {
  servo.write(0);
}