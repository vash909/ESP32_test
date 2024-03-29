/* Simple LoRa packet sender (no LoRaWAN) + BME280
by IU1BOT Lorenzo  
For Lilygo TTGO T3 V1.6.1 
*/
#include <Arduino.h>

//libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Libraries for BME280 sensor
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

 //define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

#define BAND 868E6 //LoRa frequency band (in Hz)

//OLED pins
#define OLED_SDA 21
#define OLED_SCL 22 
#define OLED_RST -1
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

//i2c startup definition for BME280
Adafruit_BME280 bme;


bool sent = 0; //sent variable
int counter = 0; //packet counter

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void setup() {
  Serial.begin(115200); //initialize Serial Monitor

  bool status; //define variable for BME280 initialize return
  status = bme.begin(0x76); //initialize BME280 on i2c address 0x76
if (!status) {
  Serial.println("Could not find a valid BME280 sensor, check wiring!");
  display.println("no BME280 on i2c!");
  for (;;); //if no BME280 is detected loop forever
  while (1);
}

  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) 
  { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    delay(5000);
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LoRa RAW Packets");
  display.display();
  
  Serial.println("LoRa RAW Packets sender");

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(BAND)) {  //LoRa starting check
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(20); //LoRa output power (1 to 20 - default: 17)
  LoRa.setGain(0); //0 to 6, 0 enables AGC, 6 is maximum value allowed
  LoRa.setSpreadingFactor(7); //LoRa SF parameter
  LoRa.setSignalBandwidth(125E3); //LoRa Bandwidth
  LoRa.setCodingRate4(5); //LoRa CR Parameter
  LoRa.setPreambleLength(8); //Preamble symbols
  
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0,10);
  display.print("LoRa Initializing OK!");
  display.setCursor(0,20);
  display.print("Loading...");
  display.display();
  delay(5000);
}

void loop() {


  display.clearDisplay();
  display.setCursor(0,0);
  display.println("LoRa RAW sender");
  display.setCursor(0,10);
  display.print("by IU1BOT");

  Serial.print("Sending packet: ");
  Serial.println(counter);
  Serial.println("temp:");
  Serial.println(bme.readTemperature());
  Serial.println("rH:");
  Serial.print(bme.readHumidity());
  Serial.print("%");
  Serial.println('\n');

  //Send LoRa packet to transceiver
  LoRa.beginPacket();
  LoRa.print("temp:");
  LoRa.print(bme.readTemperature());
  LoRa.print(" - rH:");
  LoRa.print(bme.readHumidity());
  LoRa.print("%");
  /*
  LoRa.print("Pressure = ");         //Remove comment to send pressure value via LoRa packet
  LoRa.print(bme.readPressure() / 100.0F);
  LoRa.print(" hPa");
  */
  LoRa.endPacket(sent);

  //Frequencies detail and header
  display.setCursor(0,20);
  display.setTextSize(1);
  display.print("868MHz SF7 CR5 125KHz");
  display.setCursor(0,30);
  display.print("Counter:");
  
   if(sent = 1) { 
  display.setCursor(50,30);
  display.print(counter); 
  display.setCursor(0,40);
  display.print("temp:");
  display.print(bme.readTemperature());
  display.print(" - rH:");
  display.print(bme.readHumidity());
  display.print("%");    
  display.display();

  }

  else if(sent = 0) { 
  display.setCursor(50,30);
  display.print("TX FAIL!");      
  display.display();
  }

  counter++;
  
  delay(5000);

}

