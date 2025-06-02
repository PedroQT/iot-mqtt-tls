#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "ledclient.h"
#include "ledsecrets.h"

WiFiClientSecure ledSecureClient;
PubSubClient ledMqttClient(ledSecureClient);

const int LED_PIN = 2;

void ledCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("⚡ Tópico recibido: ");
  Serial.println(topic);
  
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("📩 Mensaje: ");
  Serial.println(message);

  if (message.equalsIgnoreCase("encendido") || message.equalsIgnoreCase("on")) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("✅ LED ENCENDIDO");
  } else if (message.equalsIgnoreCase("apagado") || message.equalsIgnoreCase("off")) {
    digitalWrite(LED_PIN, LOW);
    Serial.println("🛑 LED APAGADO");
  }
}

void reconnectLedClient() {
  while (!ledMqttClient.connected()) {
    Serial.print("🔁 Conectando LED MQTT...");
    if (ledMqttClient.connect(LED_CLIENT_ID, LED_MQTT_USER, LED_MQTT_PASSWORD)) {
      Serial.println("✅ Conectado");
      ledMqttClient.subscribe(LED_MQTT_TOPIC_SUB);
      Serial.print("📡 Suscrito al tópico: ");
      Serial.println(LED_MQTT_TOPIC_SUB);
    } else {
      Serial.print("❌ Fallo: ");
      Serial.println(ledMqttClient.state());
      delay(5000);
    }
  }
}

void setupLedClient() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  ledSecureClient.setCACert(LED_ROOT_CA);
  ledMqttClient.setServer(LED_MQTT_SERVER, LED_MQTT_PORT);
  ledMqttClient.setCallback(ledCallback);
}

void loopLedClient() {
  if (!ledMqttClient.connected()) {
    reconnectLedClient();
  }
  ledMqttClient.loop();
}
