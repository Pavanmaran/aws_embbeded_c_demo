#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* WIFI_SSID     = "Me";
const char* WIFI_PASSWORD = "12345678";
const char* AWS_ENDPOINT  = "a1dkr9puunhret-ats.iot.us-east-1.amazonaws.com";
const char* CLIENT_ID     = "machineMonitor";
const char* TOPIC         = "factory/machine01/telemetry";
constexpr uint8_t LIGHT_ADC_PIN = 34;

const char* AWS_CERT_CRT = R"EOF(

)EOF";

const char* AWS_CERT_PRIVATE = R"EOF(

)EOF";

const char* AWS_CERT_CA = R"EOF(

)EOF";

WiFiClientSecure net;
PubSubClient client(net);

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}

bool connectAWS() {
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
  client.setServer(AWS_ENDPOINT, 8883);

  Serial.print("Connecting to AWS IoT");
  for (int attempt = 1; attempt <= 10; ++attempt) {
    if (client.connect(CLIENT_ID)) {
      Serial.println(" connected to AWS");
      return true;
    }
    Serial.printf("\nMQTT connect failed (attempt %d), state=%d\n", attempt, client.state());
    delay(2000);
  }
  Serial.println("AWS IoT connection failed; will retry later.");
  return false;
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetPinAttenuation(LIGHT_ADC_PIN, ADC_11db);
  connectWiFi();
  connectAWS();
}

void loop() {
  if (!client.connected()) {
    connectAWS();
  }
  client.loop();

  const int lightAdcRaw = analogRead(LIGHT_ADC_PIN);
  const float lightIntensityPct = (lightAdcRaw * 100.0f) / 4095.0f;

  char payload[160];
  snprintf(payload, sizeof(payload),
    "{\"device_id\":\"%s\",\"light_intensity_raw\":%d,\"light_intensity_pct\":%.1f}",
    CLIENT_ID, lightAdcRaw, lightIntensityPct);

  if (client.publish(TOPIC, payload)) {
    Serial.println(payload);
  } else {
    Serial.printf("MQTT publish failed, state=%d\n", client.state());
  }

  delay(2000);
}