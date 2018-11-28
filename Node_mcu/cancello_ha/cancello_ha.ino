#include "ESP8266WiFi.h"
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

// dati wifi
const char* ssid = "SSD_wifi";
const char* password = "password_wifi";

// indirizzo ip mqtt server
const char* mqtt_server = "indirizzo_ip_mtt_server";

// comando MQTT
const char* mqtt_comando = "prova/cancellino/comando";

// Pin rele pulsante
int releuno = 2;

// porta attesa Web server
ESP8266WebServer server(99);
String page = "";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Rete wifi scelta: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connessa");
  Serial.println("il tuo indirizzo IP è: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {

  if (strcmp(topic,mqtt_comando)==0){
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    if ((char)payload[0] == 'a' and (char)payload[1] == 'z' and (char)payload[2] == 'i' and (char)payload[3] == 'o' ) {

      comando_cancello();
      delay(500);
    } }

    }

void reconnect() {
  while (!client.connected()) {
    Serial.print("ricerca del server mqtt...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("OK connesso al server mqtt");
      client.subscribe(mqtt_comando);

      delay(500);
    } else {
      Serial.print("errore connessione, rc=");
      Serial.print(client.state());
      Serial.println(" ripovo tra 5 secondi....");
      // attendo 5 secondi fino al prossimo tentativo
      delay(5000);
    }}}


void setup(void) {
Serial.begin(9600);
Serial.print("Iniziamo....");
setup_wifi();
page = "<h1>Comando cancello</h1><p><a href=\"comando-cancello\"><button>AZIONE</button></a>&nbsp";

client.setServer(mqtt_server, 1883);
   client.setCallback(callback);
   reconnect();


// configuro i rele
pinMode(releuno, OUTPUT);
digitalWrite(releuno, HIGH);

server.on("/", [](){
    server.send(200, "text/html", page);
  });

server.on("/comando-cancello", []() {
  comando_cancello();
  server.send(200, "text/html", "<h1>Comando inviato</h1>");
});

// avvio il server
server.begin();
Serial.println("ESP8266 HTTP server avviato");
}

void loop() {
  if (!client.connected()) {
    reconnect();}
 client.loop();
server.handleClient();

}

void comando_cancello(){
  digitalWrite(releuno, LOW);
  delay(1000);
  digitalWrite(releuno, HIGH);
  Serial.println("cancello in apertura");
  delay (500);
}
