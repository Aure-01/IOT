#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char *ssid = "W_Aula_WB11";
const char *password = "itcolima6";
String serverName = "http://424e-187-190-35-202.ngrok-free.app";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

int contador = 0;  // Inicializa el contador en 0
const int botonAumentarPin = 4;
const int botonDisminuirPin = 5;
const int ledPin = 21;

bool ledStatus = false;

// Configura el sensor DHT11
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void post_data(String action, int quantity)
{
  DynamicJsonDocument json_chido(1024);
  json_chido["action"] = action;
  json_chido["quantity"] = quantity;

  String json_str;
  serializeJson(json_chido, json_str);

  HTTPClient http;
  http.begin(serverName+"/counter");
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

void post_asc()
{
  post_data("asc", 1);
}

void post_desc()
{
  post_data("desc", 1);
}

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(botonAumentarPin, INPUT);
  pinMode(botonDisminuirPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Inicializa el estado del LED
  digitalWrite(ledPin, ledStatus ? HIGH : LOW);

  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  dht.begin(); // Inicializa el sensor DHT11
}

void loop()
{
  int estadoBotonAumentar = digitalRead(botonAumentarPin);
  int estadoBotonDisminuir = digitalRead(botonDisminuirPin);

  if (estadoBotonAumentar == HIGH)
  {
    contador++;
    post_asc(); // Envía datos al servidor
  }
  else if (estadoBotonDisminuir == HIGH)
  {
    contador--;
    post_desc(); // Envía datos al servidor
  }

  Serial.print("Contador: ");
  Serial.println(contador);

  // Lectura de temperatura del sensor DHT11
  float temperature = dht.readTemperature();

  if (!isnan(temperature) && temperature >= -40 && temperature <= 80)
  {
    Serial.print("Temperatura: ");
    Serial.println(temperature);
    // Envía la temperatura al servidor
    postTemperature(temperature);
  }
  else
  {
    Serial.println("Error al leer la temperatura del sensor DHT11");
  }

  if ((millis() - lastTime) > timerDelay)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http;
      String serverPath = serverName;
      http.begin(serverPath.c_str());
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0)
      {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        
        // Enciende el LED cuando se recibe una solicitud GET exitosa
        digitalWrite(ledPin, HIGH);
        delay(1000);
        digitalWrite(ledPin, LOW);
      }
      else
      {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void postTemperature(float temperature)
{
  DynamicJsonDocument json_chido(1024);
  json_chido["temperature"] = temperature;

  String json_str;
  serializeJson(json_chido, json_str);

  HTTPClient http;
  http.begin(serverName + "/temperature");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(json_str);

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
    
    // Enciende el LED cuando se realiza una solicitud POST exitosa
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
