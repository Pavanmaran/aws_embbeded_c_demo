#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* WIFI_SSID     = "Me";
const char* WIFI_PASSWORD = "12345678";
const char* AWS_ENDPOINT  = "a1dkr9puunhret-ats.iot.us-east-1.amazonaws.com";
const char* CLIENT_ID     = "machineMonitor";
const char* TOPIC         = "factory/machine01/telemetry";
constexpr uint8_t LIGHT_ADC_PIN = 34;

const char* AWS_CERT_CRT = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUdSB5zHil3N3z0htn77GvU7WE1Z4wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MDkyODA5MjIz
N1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMlHx+3swMsB1622YcP/
+T0Y4ZGGuZffGJOwKn3WpeMpwP8GQ8gMz8NVxpNiVPNyVy0ZOgqWkNwreB+4UqHr
fG8KYiX/ZoIJuvRAZGn0oNHlADrTzdKjn63J0xXJbJ3uLU7KTVNgZkwAYZDIuwQ7
OX3SwqrOotqpV0CQCvdlpSFNilOwX43JkpSt6S/j8t/yj1UtT126K+uRfexbbYJK
nOGbdIojAXEaPMbpG/3dqT72pEamWXEIP3caOuJ9YMxhJouqu+Ng9Mejla6Yp7C0
kecaSkuZoZqTzv44fKhUUI3vgSQPF2IWl2mlPs2EF+viv51XK6cX783HvKFZGuBD
DWkCAwEAAaNgMF4wHwYDVR0jBBgwFoAULqJfQsZ67ICYkUGq80S0SXNJ650wHQYD
VR0OBBYEFLjvDeNVarts+T4Sfo5OsBXmytJBMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAeC7xGGNWJ6JAIZnxbU4vDtStA
MzjkDJmCEtGycHK4SuhHY6ug7pvdQhL8YuOxoC+NV3Gys4BdGVroEPIP/Unkl817
dVb4MmXASzmSGNjgXHwKlo9biRLxeceGpn7Ds2pKQJEy/34mEiuyZRDJNRZHNU+s
Yo/Jo3luF0E9ovkpDebUWji7vR/C5j/y4zf665q9QnbXynTsm3c8YRD84vqMLw4/
JxNnRC9qcTFJnBT+IG/6BFupg+TJBrxqBa+JnVxkCQ7zsNPDXRcFXkfGgxbQOc/g
nUlcxzoEy7xkIMaHph2Kkui0jNOdeduTO1umfQc2Z3dQPMbu4lMQfv+lyYyB
-----END CERTIFICATE-----
)EOF";

const char* AWS_CERT_PRIVATE = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAyUfH7ezAywHXrbZhw//5PRjhkYa5l98Yk7Aqfdal4ynA/wZD
yAzPw1XGk2JU83JXLRk6CpaQ3Ct4H7hSoet8bwpiJf9mggm69EBkafSg0eUAOtPN
0qOfrcnTFclsne4tTspNU2BmTABhkMi7BDs5fdLCqs6i2qlXQJAK92WlIU2KU7Bf
jcmSlK3pL+Py3/KPVS1PXbor65F97Fttgkqc4Zt0iiMBcRo8xukb/d2pPvakRqZZ
cQg/dxo64n1gzGEmi6q742D0x6OVrpinsLSR5xpKS5mhmpPO/jh8qFRQje+BJA8X
YhaXaaU+zYQX6+K/nVcrpxfvzce8oVka4EMNaQIDAQABAoIBAQCog2QrGc+/Jme/
k7X+Nkwgjrgq8jRnpewZmZ+5gv+XU9m0qTYOQkufKiz6L5UXzF6X70rcmnnlGq+T
PupPjau7+2F4ln7n1+WhoFdxLdGGaMbseW8ULtD82r/SubQp9MIR+OUMLOT4NEl6
wdNxE3Rqlw1Lm3xUvPnDn9TOk1bQrXI2IkqPQrzxEYuF0fUiks6OGXCSqjHkq/QN
bJdDLvUNfxMC12L1+uSqgUZCSNjwzDzQvrJSgpPs3KEVR2uB5qvJyV8NO0A6tnX3
8Qc+BjlsXvf2e0qW6lIvOHl9ImXr1o/cjrhCJgw2s8YginrWh2F2OmLTve6plTj9
xDAqgOwBAoGBAOgXAco6NyNgXPeDKBPajs1FMZQovStOll+4gA/Q+oOuNLrF0isw
+BieQh4mAvs5uVcgyCYkxPINqCI6zGCLW6ReNPVg5FMnbfR62OKjB/ttmWVXR4TM
crVe5j8wtj9cx+6TSHpdKZNZe4ketzXJnRG8k1LcWDNO5vqofORbJ2PhAoGBAN4E
OWBo4Qq5TAOmcALVM8BqdiEWcVhCg0xeSyKk33wmVitSdd/fzhuyrLLp1LFMJzv0
adKLHZeBqup+G6M2gQysYavnICF+zxLPpUWqFOqYXsbhrg9biM4a0gWtFl+ClDFC
yjYlFw1ydfs8/Z2BYxLMwk/CLdr0sqXtJyxRQdqJAoGBAMMyLmg15A/VyPrGjVt9
UuMjkdopFUiqFOC2nMYwnbFIP8LiA6wdyFt0JumSUKmYV+AGiy9Bd36Yq9L2HtQI
APBR5zqE3iNtoxWeiHC8a0c0W3IXIOh4xvTxtrZosq7ByslFiAyZ9BCgjry5X59W
TMsRDdr/xRKKBsvEDx5D5/IhAoGBAJzHztzOM10nTQ6m0iKt/EW86H6lIpV5Is8j
J1vnogpB6Effi2301c+pdVtC1Pf/FxW3m6MaqmL7Jn9BXrL7IlaYxMBoDsCYlBJs
oAF6AhtITmWTqR8fPJSPTBDGEi6QP6YpzDJALu9IDRjGETLQ0+3Kus0oCsm8Uwvp
VOhu2VpBAoGAYrXLtzN+YAETp5hi4NcBp3eUuR+X1kuc27MgY99WWL0HRUBKMjCW
9wM0DAhNGcjMAcvUp86TI1iHzbZQsYofRQBPh++luZiZhg2+J3EMxPBZ57AV9QEO
zs3/xCr9thxhiY6NwRgpk7Cw0vpXR90C65l3lGVJG5ipPc96UnmNb28=
-----END RSA PRIVATE KEY-----
)EOF";

const char* AWS_CERT_CA = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
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