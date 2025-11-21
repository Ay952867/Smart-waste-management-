#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
unsigned long myChannelNumber = YOUR_CHANNEL_ID;     // e.g. 1234567
const char* myWriteAPIKey = "YOUR_API_KEY";          // e.g. ABCDEFGHIJKL1234

// Ultrasonic sensor pins
#define TRIG_PIN D5
#define ECHO_PIN D6

WiFiClient client;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  ThingSpeak.begin(client);
}

void loop() {
  // Send a short pulse to trigger the ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo time and calculate distance
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2; // in cm

  // Suppose bin height = 30 cm, calculate fill level
  float binHeight = 30.0;
  float level = binHeight - distance; // how much filled
  if (level < 0) level = 0;
  if (level > binHeight) level = binHeight;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Level: ");
  Serial.print(level);
  Serial.println(" cm");

  // Send data to ThingSpeak (Field1)
  ThingSpeak.setField(1, level);

  int status = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (status == 200) {
    Serial.println("Data sent to ThingSpeak successfully!");
  } else {
    Serial.println("Failed to send data. HTTP error code: " + String(status));
  }

  delay(20000); // ThingSpeak allows update every 15 sec minimum
}
