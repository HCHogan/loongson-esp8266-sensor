#include <ESP8266WiFi.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include <math.h>
#include <Wire.h>
#include <Adafruit_SGP30.h>

const char* ssid = "Redmi";
const char* password = "20040528";
const char* server_ip = "192.168.173.146";
const int server_port = 80;
const int ID = 2;

DHT dht(2, DHT11);
SoftwareSerial serial1(10, 11);
Adafruit_SGP30 sgp30;

int co2, tvoc, lux;

int BH1750address = 0x23; 

byte buff[2];


int BH1750_Read(int address) //

{

  int i=0;

  Wire.beginTransmission(address);

  Wire.requestFrom(address, 2);

  while(Wire.available()) //

  {

    buff[i] = Wire.read();  // receive one byte

    i++;

  }

  Wire.endTransmission();  

  return i;

}




void BH1750_Init(int address) 

{

  Wire.beginTransmission(address);

  Wire.write(0x10);//1lx reolution 120ms

  Wire.endTransmission();

}

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  dht.begin();
  Wire.begin(4, 5);
  
  if (! sgp30.begin()){
    Serial.println("SGP30 not found");
    while (1);
  }
  Serial.println("SGP30 initialized");
  BH1750_Init(BH1750address);
  
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
  if (! sgp30.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }

  co2 = sgp30.eCO2;
  tvoc = sgp30.TVOC;

  if(2==BH1750_Read(BH1750address)) {

    lux=((buff[0]<<8)|buff[1])/1.2;

    Serial.print(lux,DEC);     

    Serial.print("[lx] "); 
  }
  Serial.print(" CO2: ");
  Serial.print(co2);
  Serial.print(" ppm\tTVOC: ");
  Serial.print(tvoc);
  Serial.print(" ppb ");
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
    client.print("&co2=");
    client.print(co2);
    client.print("&tvoc=");
    client.print(tvoc);
    client.print("&lux=");
    client.print(lux);
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

  delay(500);
}
