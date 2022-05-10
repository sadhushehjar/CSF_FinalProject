#include <Wire.h>
#include <ESP8266WiFi.h>
#include<PubSubClient.h>

// Wifi Credentials.
char* ssid = "WBL";//"WBL";//
char* password = "B10s3nsors"; //"B10s3nsors";
#define MSG_BUFFER_SIZE (50)
WiFiClient espClient;
char payload_buffer[70];
PubSubClient client(espClient);

// public broker for now.
const char* mqtt_server = "test.mosquitto.org";//"";
char msg[MSG_BUFFER_SIZE];
int value = 0;
long lastMsg = 0;
int samp_hz = 250;
const unsigned long READ_PERIOD = 4000;  // 4000 us: 250 Hz
unsigned long prev_micros = 0;
unsigned long prev_micros_clientloop = 0;

long int sample_counter = 0;

// Define publisher route.
const char* MQTT_PUB_VOLT0 = "/csf/ecg/";
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
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

void callback(char* topic, byte* payload , unsigned int length ) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();

}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(100);
    }
  }
}
void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(14, INPUT); // Setup for leads off detection LO +
  pinMode(12, INPUT); // Setup for leads off detection LO -
}

void loop() {
  unsigned long curr_micros = micros();

  // handel .
  if (!client.connected()) {
    reconnect();
  }

  if (curr_micros - prev_micros >= READ_PERIOD) {// READ_PERIOD = 4000us
    //Serial.println("\n WiFi Mac Adress \n");
    //Serial.println(WiFi.macAddress());
    //Serial.print("Analog ECG read (Payload): \n");
//    Serial.print("\n");
    sample_counter+=1;
    sprintf(payload_buffer, "%s,%d, %lu,%lu\n\0",MQTT_PUB_VOLT0,analogRead(A0),curr_micros,sample_counter);
//This takes a lot of time.
//    String data = String(MQTT_PUB_VOLT0) + ","+String(WiFi.macAddress())+ "," + String(analogRead(A0)) + "," + String(curr_micros);
    Serial.print(payload_buffer);
    client.publish(MQTT_PUB_VOLT0, payload_buffer);
    prev_micros = curr_micros;
  }
   if (curr_micros-prev_micros_clientloop >= READ_PERIOD*500){
       client.loop();
       prev_micros_clientloop=curr_micros;
    }
}
