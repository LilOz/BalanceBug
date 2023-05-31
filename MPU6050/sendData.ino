#include <Wire.h>
#include <MPU6050_tockn.h>
#include <SPI.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include <ArduinoHttpClient.h>
char ssid[] = "96 Dalling Road";     // replace with your network SSID
char pass[] = "Panda123"; // replace with your network password
// http://18.207.187.97/
char server[] = "18.207.187.97";  // replace with the server address
byte ip[] = { 18,207,187,97};
JSONVar readings;
JSONVar readings2;
WiFiClient wifi;
HttpClient client = HttpClient(wifi, server, 3000);
unsigned long lastTime = 0;
unsigned long lastTimeTemperature = 0;
unsigned long lastTimeAcc = 0;
unsigned long gyroDelay = 10;
unsigned long temperatureDelay = 1000;
unsigned long accelerometerDelay = 200;
MPU6050 mpu6050(Wire);
float gyroX, gyroY, gyroZ;
float accX, accY, accZ;
float temperature;
String getGyroReadings() {
  float gyroX = mpu6050.getAngleX();
  float gyroY = mpu6050.getAngleY();
  float gyroZ = mpu6050.getAngleZ();
  readings2["gyroX"] = String(gyroX);
  readings2["gyroY"] = String(gyroY);
  readings2["gyroZ"] = String(gyroZ);
  String js = JSON.stringify(readings2);
  return js;
}
String getAccReadings() {
  float accX = mpu6050.getAccX();
  float accY = mpu6050.getAccY();
  float accZ = mpu6050.getAccZ();
  readings["accX"] = String(accX);
  readings["accY"] = String(accY);
  readings["accZ"] = String(accZ);
  String accString = JSON.stringify(readings);
  return accString;
}
void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    int status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();
  Serial.println("\nStarting connection to server...");
}
void sendData(String a,String b) {
  if (!client.connected()) {
    Serial.println("Connecting to server...");
    if (client.connect(ip, 3000)) {
      Serial.println("Connected to server");
    } else {
      Serial.println("Connection failed"); 
    }
  }
  else{
    String postData = a;
    client.beginRequest();
    client.post("/acc", "application/json", postData);
    String response = client.responseBody();
    Serial.println(postData);
    String gyroData = b;
    client.beginRequest();
    client.post("/gyr", "application/json", gyroData);
    Serial.println(gyroData);
  }  
}
void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
void loop() {
  mpu6050.update();
  String a = getAccReadings();
  String b = getGyroReadings();
  Serial.print(a);
  Serial.print(b);
  sendData(a,b);
  delay(50);
  }
