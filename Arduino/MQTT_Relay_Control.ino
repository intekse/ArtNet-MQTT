#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* mqtt_server = "192.168.1.102";
#define LED 2
#define RELAY1 2
#define RELAY2 13
#define StatusUpdateDelay 2000
#define scene "livingRoom/tree"
#define outTopic "church/relay1/status"
#define inTopic "church/relay1/value"
#define relay1Topic "church/relay1/value"
#define relay2Topic "church/relay2/value"

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int Relay1Status = 0;
int Relay2Status = 0;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  /*
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  */

  if(String(topic) == String("church/relay1/value")) {
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      digitalWrite(RELAY1, LOW);
      Relay1Status = 1;
    } else {
      digitalWrite(RELAY1, HIGH);
      Relay1Status = 0;
    }
  }
  if(String(topic) == String("church/relay2/value")) {
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      digitalWrite(RELAY2, LOW);
      Relay2Status = 1;
    } else {
      digitalWrite(RELAY2, HIGH);
      Relay2Status = 0;
    }
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "Reconnecting");
      // ... and resubscribe
      //client.subscribe(inTopic);
      client.subscribe(relay1Topic);
      client.subscribe(relay2Topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(RELAY1, OUTPUT);     // Initialize the RELAY 1 pin as an output
  pinMode(RELAY2, OUTPUT);     // Initialize the RELAY 2 pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  /*
  if (now - lastMsg > StatusUpdateDelay) {
    lastMsg = now;
    Serial.println("Updating status topic: ");
    snprintf (msg, 75, "%s = %d", scene, Relay1Status);
    client.publish(outTopic, msg);
  }
  */
}
