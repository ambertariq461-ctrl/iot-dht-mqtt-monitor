// IoT sensor node with alarm: ESP32 + DHT22 + buzzer, publishing to MQTT (Wokwi)
// Libraries (Wokwi Library Manager / libraries.txt): "PubSubClient" and "DHT sensor library"
// Wiring: DHT22 SDA -> D15 | Buzzer positive (pin 2) -> D13 | Buzzer negative (pin 1) -> GND

#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// ---- Sensor ----
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ---- Buzzer ----
const int BUZZER_PIN = 13;

// ---- Alarm limits (change these to test) ----
const float TEMP_LIMIT = 35.0;  // degrees C: alarm above this
const float HUM_LIMIT  = 70.0;  // percent: alarm above this

// ---- Network (Wokwi provides a simulated WiFi called Wokwi-GUEST) ----
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";

// ---- MQTT (public test broker: anyone can read your topic, so send no private data) ----
const char* MQTT_SERVER = "broker.hivemq.com";
const int   MQTT_PORT   = 1883;
const char* MQTT_TOPIC  = "iot-DHT-4827/sensor";  // change to your own unique name if you like

WiFiClient espClient;
PubSubClient mqtt(espClient);
unsigned long lastPublish = 0;
const unsigned long PUBLISH_EVERY_MS = 1000;

// Make a beep by switching the buzzer pin on and off about 1000 times a second
void beep(unsigned long durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(500);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(500);
  }
}

void alarmBeeps() {
  for (int i = 0; i < 3; i++) {
    beep(150);
    delay(100);
  }
}

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}

void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT... ");
    String clientId = "esp32-" + String(random(0xFFFF), HEX);
    if (mqtt.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  dht.begin();
  connectWiFi();
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();

  if (millis() - lastPublish >= PUBLISH_EVERY_MS) {
    lastPublish = millis();

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Sensor read failed");
      return;
    }

    bool tempHigh = temperature > TEMP_LIMIT;
    bool humHigh  = humidity > HUM_LIMIT;
    bool alarm = tempHigh || humHigh;

    char payload[80];
    snprintf(payload, sizeof(payload),
             "{\"temp\":%.1f,\"hum\":%.1f,\"alarm\":%d}",
             temperature, humidity, alarm ? 1 : 0);

    mqtt.publish(MQTT_TOPIC, payload);
    Serial.print("Published: ");
    Serial.println(payload);

    if (alarm) {
      if (tempHigh) Serial.println("ALARM: temperature too high");
      if (humHigh)  Serial.println("ALARM: humidity too high");
      alarmBeeps();
    }
  }
}
