#include <WiFi.h>
#include <HTTPClient.h>

// Replace with your network credentials
const char *ssid = "W_Aula_WB11";
const char *password = "itcolima6";

// Replace with the URL of the resource you want to get
const char *url = "http://6c42-187-190-35-202.ngrok-free.app/led";

void setup()
{
  Serial.begin(115200);

  // Connect to the Wi-Fi network
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
}

void loop()
{
  // Create an HTTPClient object
  HTTPClient http;

  // Begin the HTTP GET request
  http.begin(url);

  // Send the request
  int httpCode = http.GET();

  // Check the response code
  if (httpCode > 0)
  {
    // Success!
    Serial.println("HTTP GET request successful!");

    // Get the response payload
    String payload = http.getString();

    // Print the payload to the serial monitor
    Serial.println(payload);
  }
  else
  {
    // Error!
    Serial.println("HTTP GET request failed!");
    Serial.println(httpCode);
  }

  // Close the HTTP connection
  http.end();

  // Delay for 1 second
  delay(1000);
}
