#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

// WiFi
const char *ssid = "Cuarto de Aure_5G";   // Enter your WiFi name
const char *password = "2WC456400946"; // Enter WiFi password


// MQTT Broker
const char *mqtt_broker = "w3a4bbd9.ala.us-east-1.emqxsl.com";    // broker address
const char *mqttTopic = "monitores/#";                            // define topic
const char *mqtt_username = "esp32";                              // username for authentication
const char *mqtt_password = "password";                           // password for authentication
const int mqtt_port = 8883;                                       // port of MQTT over TLS/SSL

// load DigiCert Global Root CA ca_cert
const char *ca_cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIEqjCCA5KgAwIBAgIQAnmsRYvBskWr+YBTzSybsTANBgkqhkiG9w0BAQsFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
    "QTAeFw0xNzExMjcxMjQ2MTBaFw0yNzExMjcxMjQ2MTBaMG4xCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
    "b20xLTArBgNVBAMTJEVuY3J5cHRpb24gRXZlcnl3aGVyZSBEViBUTFMgQ0EgLSBH\n"
    "MTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALPeP6wkab41dyQh6mKc\n"
    "oHqt3jRIxW5MDvf9QyiOR7VfFwK656es0UFiIb74N9pRntzF1UgYzDGu3ppZVMdo\n"
    "lbxhm6dWS9OK/lFehKNT0OYI9aqk6F+U7cA6jxSC+iDBPXwdF4rs3KRyp3aQn6pj\n"
    "pp1yr7IB6Y4zv72Ee/PlZ/6rK6InC6WpK0nPVOYR7n9iDuPe1E4IxUMBH/T33+3h\n"
    "yuH3dvfgiWUOUkjdpMbyxX+XNle5uEIiyBsi4IvbcTCh8ruifCIi5mDXkZrnMT8n\n"
    "wfYCV6v6kDdXkbgGRLKsR4pucbJtbKqIkUGxuZI2t7pfewKRc5nWecvDBZf3+p1M\n"
    "pA8CAwEAAaOCAU8wggFLMB0GA1UdDgQWBBRVdE+yck/1YLpQ0dfmUVyaAYca1zAf\n"
    "BgNVHSMEGDAWgBQD3lA1VtFMu2bwo+IbG8OXsj3RVTAOBgNVHQ8BAf8EBAMCAYYw\n"
    "HQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8C\n"
    "AQAwNAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdp\n"
    "Y2VydC5jb20wQgYDVR0fBDswOTA3oDWgM4YxaHR0cDovL2NybDMuZGlnaWNlcnQu\n"
    "Y29tL0RpZ2lDZXJ0R2xvYmFsUm9vdENBLmNybDBMBgNVHSAERTBDMDcGCWCGSAGG\n"
    "/WwBAjAqMCgGCCsGAQUFBwIBFhxodHRwczovL3d3dy5kaWdpY2VydC5jb20vQ1BT\n"
    "MAgGBmeBDAECATANBgkqhkiG9w0BAQsFAAOCAQEAK3Gp6/aGq7aBZsxf/oQ+TD/B\n"
    "SwW3AU4ETK+GQf2kFzYZkby5SFrHdPomunx2HBzViUchGoofGgg7gHW0W3MlQAXW\n"
    "M0r5LUvStcr82QDWYNPaUy4taCQmyaJ+VB+6wxHstSigOlSNF2a6vg4rgexixeiV\n"
    "4YSB03Yqp2t3TeZHM9ESfkus74nQyW7pRGezj+TC44xCagCQQOzzNmzEAP2SnCrJ\n"
    "sNE2DpRVMnL8J6xBRdjmOsC3N6cQuKuRXbzByVBjCqAA8t1L0I+9wXJerLPyErjy\n"
    "rMKWaBFLmfK/AHNF4ZihwPGOc7w6UHczBZXH5RFzJNnww+WnKuTPI0HfnVH8lg==\n"
    "-----END CERTIFICATE-----\n";

// init secure wifi client
WiFiClientSecure espClient;
// use wifi client to init mqtt client
PubSubClient client(espClient);

int valor = 0; // Este es el valor que incrementarás o decrementarás

// Pines de los botones
const int pinBotonIncrementar = 4;  
const int pinBotonDecrementar = 5;  
const int pinLED = 21;

// Configuración del sensor DHT11
#define DHTPIN 2  // Ejemplo, ajusta según tu configuración
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

unsigned long tiempoAnterior = 0;
const long intervaloEnvio = 30000;

void setup()
{
  // Set software serial baud to 115200;
  Serial.begin(115200);
  // Inicializa los pines de los botones como entrada
  pinMode(pinBotonIncrementar, INPUT_PULLUP);
  pinMode(pinBotonDecrementar, INPUT_PULLUP);
  pinMode(pinLED, OUTPUT);
  dht.begin();
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  // set root ca cert
  espClient.setCACert(ca_cert);
  // connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected())
  {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println("Retrying in 5 seconds.");
      delay(5000);
    }
  }
  // publish and subscribe
  client.publish(mqttTopic, "Hi EMQX I'm ESP32 ^^"); // publish to the topic
  client.subscribe(mqttTopic);                       // subscribe from the topic
}
void loop() {
  // Lee el estado de los botones
  int estadoBotonIncrementar = digitalRead(pinBotonIncrementar);
  int estadoBotonDecrementar = digitalRead(pinBotonDecrementar);

  // Incrementa o decrementa el valor según el estado de los botones
  if (estadoBotonIncrementar == HIGH) {
    incrementarValor();
    delay(500);  // Evita múltiples incrementos rápidos con un pequeño retardo
  }

  if (estadoBotonDecrementar == HIGH) {
    decrementarValor();
    delay(500);  // Evita múltiples decrementos rápidos con un pequeño retardo
  }

  // Verifica el intervalo de envío para los datos del sensor DHT11
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoAnterior >= intervaloEnvio) {
    // Lee datos del sensor DHT11
    float temperatura = dht.readTemperature();
    float humedad = dht.readHumidity();

    // Publica los datos en el tema MQTT
    enviarDatosDHT(temperatura, humedad);

    // Actualiza el tiempo anterior
    tiempoAnterior = tiempoActual;
  }

  // Continúa con el loop del cliente MQTT
  client.loop();
}

void callback(char *topic, byte *payload, unsigned int length)
{
  // Este es el manejador de mensajes MQTT de entrada
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  // Convierte el payload en una cadena
  String message;
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  Serial.print("Mensaje: ");
  Serial.println(message);

  // Create a dynamic JSON document based on the payload size
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, message);

  // Verifica si el mensaje contiene la propiedad "led"
  if (doc.containsKey("led")) {
    int estadoLED = doc["led"];
    if (estadoLED == 0) {
      digitalWrite(pinLED, LOW);  // Apagar LED
    } else if (estadoLED == 1) {
      digitalWrite(pinLED, HIGH);  // Encender LED
    }
  }
}

void enviarValor(int valor) {
  // Crea un objeto JSON con el formato requerido
  DynamicJsonDocument doc(1024);
  doc["from"] = "esp32";
  doc["to"] = "broadcast";
  doc["action"] = "UPDATE_COUNTER";
  doc["value"] = valor;

  // Convierte el objeto JSON en una cadena
  String jsonString;
  serializeJson(doc, jsonString);

  // Imprime el mensaje JSON en la consola
  Serial.print("Enviando JSON: ");
  Serial.println(jsonString);

  // Publica el mensaje JSON en el tema MQTT
  client.publish("monitores/#", jsonString.c_str());
}

void enviarDatosDHT(float temperatura, float humedad) {
  // Crea un objeto JSON con los datos del sensor DHT11
  DynamicJsonDocument doc(1024);
  doc["from"] = "esp32";
  doc["to"] = "server";
  doc["action"] = "SEND_DATA";
  JsonObject data = doc.createNestedObject("data");
  data["temperature"] = temperatura;
  data["humidity"] = humedad;

  // Convierte el objeto JSON en una cadena
  String jsonString;
  serializeJson(doc, jsonString);

  // Publica el mensaje JSON en el tema MQTT
  client.publish(mqttTopic, jsonString.c_str());
}

void incrementarValor() {
  valor++;
  enviarValor(valor);
}

void decrementarValor() {
  valor--;
  enviarValor(valor);
}