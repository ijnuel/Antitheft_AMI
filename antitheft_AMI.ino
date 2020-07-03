#include <ESP8266WiFi.h>            //Library for WiFi
#include <Wire.h>                   //Library for LCD I2C connection
#include <FirebaseArduino.h>        //Library for Google firebase connection
#include <ArduinoJson.h>            //Library for Converting data to JSON file
#include <LiquidCrystal_I2C.h>      //Library for LCD I2C connection
#include <Adafruit_MCP3008.h>       //Library for ADC converter
#include <SPI.h>                    //Library for ADC converter

/*Varialbles for firebase*/
#define FIREBASE_HOST "advancedmeteringinfrastr-901c3.firebaseio.com"
#define FIREBASE_AUTH "ogN3A0sciNdHhTmgfRCxoBE837WJ1tQYQEOu6BCg"

/*Variables for WiFi connection*/
#define WIFI_SSID2 "Dr Babalola"
#define WIFI_PASSWORD2 "Olufiso@2019feb"
#define WIFI_SSID1 "Infinix NOTE 3"
#define WIFI_PASSWORD1 "raphael2019"
#define WIFI_SSID "Jaydee"
#define WIFI_PASSWORD "12345677"

/*Declaration of variables for calculation*/
const int relayPin = D7;
float voltage;
float used_energy;
float start_time;
float end_time;
float app_start;
float app_stop;
float available_energy;
float time_used;
float current;
float power;
float initial_current;
float initial_voltage;
float acc;
float zero;
float zero_volt;
int j;


LiquidCrystal_I2C lcd(0x27, 20, 4);   //lcd declaration
Adafruit_MCP3008 adc;                 //adc declaration

void setup() {
  pinMode(relayPin, OUTPUT);      // set relayPin as output
  pinMode(D8, OUTPUT);      // set relayPin as output
  digitalWrite(relayPin, HIGH);   // cur off power temporarily for calibration
  Serial.begin(9600);             // initialize serial monitor
  adc.begin(D4, D2, D3, D1);      //initialize adc

  /*Initialize LCD*/
  Wire.begin(D5, D6);
  lcd.init();
  lcd.backlight();
  lcd.print("Calibrating.......");

  /*Calibrate current*/
  for (int i = 0; i < 100; i++) {
    acc += adc.readADC(0);
  }
  zero = acc / 100.0;

  /*Calibrate voltage*/
  acc = 0;
  for (int i = 0; i < 100; i++) {
    acc += adc.readADC(1);
  }
  zero_volt = acc / 100.0;

  lcd.clear();
  lcd.print("Calibration done!");

  /*Connect WiFi*/
  j = 0;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lcd.clear();
  lcd.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    lcd.print(".");
    delay(500);
    j += 1;
    if (j == 10) {
      j = 0;
      WiFi.begin(WIFI_SSID2, WIFI_PASSWORD2);
      while (WiFi.status() != WL_CONNECTED) {
        lcd.print(".");
        delay(500);
        j += 1;
        if (j == 10) {
          WiFi.begin(WIFI_SSID1, WIFI_PASSWORD1);
          while (WiFi.status() != WL_CONNECTED) {
            lcd.print(".");
            delay(500);
            j += 1;
          }
        }
      }
    }
  }
  lcd.clear();
  lcd.print("Connected to: ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  digitalWrite(relayPin, LOW);      //reconnect power


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   //connect to firebase
  app_start = millis();

}

void loop() {

  while (WiFi.status() != WL_CONNECTED) {
    j = 0;
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    lcd.clear();
    lcd.print("Connecting WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      lcd.print(".");
      delay(500);
      j += 1;
      if (j == 10) {
        j = 0;
        WiFi.begin(WIFI_SSID2, WIFI_PASSWORD2);
        while (WiFi.status() != WL_CONNECTED) {
          lcd.print(".");
          delay(500);
          j += 1;
          if (j == 10) {
            j = 0;
            WiFi.begin(WIFI_SSID1, WIFI_PASSWORD1);
            while (WiFi.status() != WL_CONNECTED) {
              lcd.print(".");
              delay(500);
              j += 1;
            }
          }
        }
      }
    }
  }

  /*Get value previous used and available energy from firebase*/
  FirebaseObject e_data = Firebase.get("/Energy_Data/User_1");
  JsonObject& ene_data = e_data.getJsonVariant();
  float ava_ene = ene_data["energyAvailable"];
  float diff = ava_ene - available_energy;
  if (ene_data["energyAvailable"] != 0) {
    available_energy = ene_data["energyAvailable"];
    used_energy = ene_data["energyConsumed"];
  }



  /*Calculate voltage value*/
  acc = 0;
  for (int i = 0; i < 100; i++) {
    acc += adc.readADC(1) - zero_volt;
  }
  voltage = (((5.0 / 1024.0) * acc / 100.0) * 42.537) + 270;
  if (voltage < 0 || voltage < 280 || voltage > 360) {
    voltage = 0.00;
  }

  /*Calculate value  of current*/
  acc = 0;
  for (int i = 0; i < 100; i++) {
    acc += adc.readADC(0) - zero;
  }
  current = acc / 100.0 / 1024.0 * 5.0 / 0.066;
  if (current < 0.08) {
    current = 0.00;
  }

  if (voltage == 0) {
    current = 0;
    digitalWrite(D8, LOW);
  }
  else {
    digitalWrite(D8, HIGH);
  }

  /*Calculate power and energy consumption*/
  power = current * voltage / 1000;
  end_time = millis();
  time_used = end_time - start_time;
  start_time = millis();
  used_energy = used_energy + (power * time_used / 1000);
  available_energy = available_energy - (power * time_used / 1000);

  /*Store all calculated variables to a JSON file and upload to Firebase*/
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& energy_data = jsonBuffer.createObject();
  energy_data["voltageInput"] = voltage;
  energy_data["current"] = current;
  energy_data["power"] = power;
  energy_data["energyConsumed"] = used_energy;
  energy_data["energyAvailable"] = available_energy;
  Firebase.set("/Energy_Data/User_1", energy_data);

  app_stop = millis();
  if (app_stop - app_start >= 5000) {
    app_start = millis();
    StaticJsonBuffer<200> jsonBuffer1;
    JsonObject& energy_data1 = jsonBuffer1.createObject();
    energy_data1["voltageInput"] = String(voltage);
    energy_data1["current"] = String(current);
    energy_data1["power"] = String(power);
    energy_data1["energyConsumed"] = String(used_energy);
    energy_data1["energyAvailable"] = String(available_energy);
    Firebase.push("/Energy_Data1/User_1", energy_data1);
    energy_data1.prettyPrintTo(Serial);
  }

  /*for LCD display*/
  if (available_energy == 0 || available_energy < 0) {  //Whenever the energy is exhausted
    available_energy = 0;
    digitalWrite(relayPin, HIGH);
    lcd.clear();
    lcd.setCursor (0, 0);
    lcd.print("Energy exhausted!!!");
    lcd.setCursor (0, 1);
    lcd.print("Purchase energy!!!");
    lcd.setCursor (0, 2);
    lcd.print("Enr Used: ");
    lcd.print(used_energy, 1);
    lcd.print("kWs");
    lcd.setCursor (0, 3);
    lcd.print("Available: ");
    lcd.print(available_energy, 1);
    lcd.print("kWs");
  }
  else {                                  //Whenever energy is available, keep displaying energy, current and power usage
    lcd.clear();
    lcd.setCursor (0, 0);
    lcd.print("Current:");
    lcd.print(current, 2);
    lcd.print("A");
    lcd.setCursor (0, 1);
    lcd.print("Voltage:");
    lcd.print(voltage, 0);
    lcd.print("V" );
    lcd.setCursor(15, 0);
    lcd.print("Power");
    lcd.setCursor (15, 1);
    lcd.print(power * 1000, 0);
    lcd.print("W");
    lcd.setCursor (0, 2);
    lcd.print("Enr Used: ");
    lcd.print(used_energy, 1);
    lcd.print("kWs");
    lcd.setCursor (0, 3);
    lcd.print("Available: ");
    lcd.print(available_energy, 1);
    lcd.print("kWs");
    digitalWrite(relayPin, LOW);
  }
  delay(1500);
}
