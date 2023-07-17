#include <ESP8266WiFi.h>
#include <DHT.h>
#include <SoftwareSerial.h>

const char* ssid = "Redmi";
const char* password = "20040528";
const char* server_ip = "192.168.173.79";
const int server_port = 80;
const int ID = 1;

DHT dht(2, DHT11);
SoftwareSerial serial1(10, 11);

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read data from DHT sensor!");
    return;
  }
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" *C, Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\n");

  WiFiClient client;
  if (client.connect(server_ip, server_port)) {
    Serial.println("Connected to server!");
    client.print("GET /data");
    client.print("?temperature=");
    client.print(temperature);
    client.print("&humidity=");
    client.print(humidity);
    client.print("&ID=");
    client.print(ID);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server_ip);
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("Connection failed!");
  }

  serial1.print("Temperature: ");
  serial1.print(temperature);
  serial1.print(" *C, Humidity: ");
  serial1.print(humidity);
  serial1.print(" %\n");

  delay(5000);
}
