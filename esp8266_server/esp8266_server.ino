#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

const char* ssid = "Redmi";
const char* password = "20040528";
WiFiServer server(80);
SoftwareSerial serial1(10, 11);

void setup() {
  Serial.begin(115200);
  serial1.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
  server.begin();
  Serial.print("Server started on ");
  Serial.print(WiFi.localIP());
  Serial.println("!");
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    // Serial.println("New client connected!");
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n') {
          break;
        }
      }
    }
    // Serial.println(request);
    int index_temperature = request.indexOf("temperature=");
    int index_humidity = request.indexOf("&humidity=");
    int index_ID = request.indexOf("&ID=");
    if (index_temperature != -1 && index_humidity != -1) {
      String temperature_str = request.substring(index_temperature + 12, index_humidity);
      String humidity_str = request.substring(index_humidity + 10, index_ID);
      String ID_str = request.substring(index_ID + 4);
      float temperature = temperature_str.toFloat();
      float humidity = humidity_str.toFloat();
      int id = ID_str.toInt();
      Serial.print("!");
      Serial.print(temperature);
      Serial.print(" ");
      Serial.print(humidity);
      
      Serial.print(" ");
      Serial.print(id);
      Serial.print(" ");
      Serial.print(client.remoteIP());
      Serial.println();
  
      serial1.print("Temperature: ");
      serial1.print(temperature);
      serial1.print(" *C, Humidity: ");
      serial1.print(humidity);
      serial1.print(" %\n");
    }
    client.stop();
    // Serial.println("Client disconnected!");
  }
}
