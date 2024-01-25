// Maak gebruik van de ESP8266 Wifi Library
#include <ESP8266WiFi.h>

// Maak gebruik van de MQTT Library
#include <PubSubClient.h>

// Library voor het gebruik van I2C apparaten ( SDA en SCL pinnen )
#include <Wire.h>

// Maak gebruik van de Grove-LCD RGB Backlight Library
#include <rgb_lcd.h>


// Maak gebruik van onderstaande Wifi credentials
//const char* ssid = "iPhone11 Roger Keijers";
//const char* password = "welkom01";
const char* ssid = "Roger_thuis";
const char* password = "Roger4572!";

// Vul het IP-adres van je Raspberry Pi in ( = MQTT-broker )
//const char* mqtt_server = "172.20.10.9";
const char* mqtt_server = "192.168.178.55";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

rgb_lcd lcd;

void setup() {
    // Geef het aantal kolommen en rijen op van het LCD-scherm:
    lcd.begin(16, 2);

    // Print onderstaand bericht op de eerste rij van het LCD-scherm. Maximaal 16 tekens.
    lcd.print("GTSv1.2");

    delay(1000);

  Serial.begin(115200);
  setup_wifi();

  // Maak een verbinding met de MQTT-broker op poort 1883 (mqtt_server)
  client.setServer(mqtt_server, 1883);
  
  // Koppel de callback function aan de verbinding.
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  
  // Maak een verbinding met het Wifi netwerk en print dit in de Serial Monitor.
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Deze function gaat de berichten van de MQTT server ophalen.
// Berichten worden gelijk op het LCD-scherm getoond.

void callback(char* topic, byte* payload, unsigned int length) {

  for (int i = 0; i < length; i++) {
    lcd.print((char)payload[i]);
  }

}

// Deze functie met een verbinding met de MQTT-broker. Ook als deze verbroken is.
void reconnect() {
  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str(),"mqtt_sub","subpass")) {
      Serial.println("connected");

    // Subscribe op een MQTT Topic.
      client.subscribe("ArduinoUnoR3/GTSv1.2/Temperatuur");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Verplaats de cursor van het LCD-scherm naar de tweede regel.
  lcd.setCursor(0, 1);
  
  // Daarna wordt de callback function aangeroepen.
  // De callback function leest het bericht van de MQTT-broker
  // De callback function zet deze karakter voor karater op het scherm.
  client.setCallback(callback);

  // Vertaging van 0.1 seconden in deze loop (100 milliseconden)
  delay(100);

  }
