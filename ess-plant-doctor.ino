#include <M5StickC.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define BLYNK_PRINT Serial

char auth[] = "your-api-key";
char ssid[] = "your-wifi-ssid";
char pass[] = "your-wifi-pass";

// GPIO where the DS18B20 is connected to
const int oneWireBus = 26;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

bool triggered = false;

uint16_t analogRead_value = 0;
uint16_t digitalRead_value = 0;
float temperature_value = 0;

void setup() {
  Serial.begin(9600);
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setCursor(25, 2);
  M5.Lcd.printf("Plant Doctor\n");
  pinMode(32, INPUT);
  M5.Lcd.setCursor(10, 30);
  M5.Lcd.print("Moisture:");
  M5.Lcd.setCursor(10, 50);
  M5.Lcd.print("Temperature:");
  
  sensors.begin();
  Blynk.begin(auth, ssid, pass);
}

bool threshold = false;
BLYNK_WRITE(V0)
{
  threshold = param.asInt();
}

void loop() {
  Serial.println("Test1");
  Blynk.run();

  sensors.requestTemperatures(); 
  M5.Lcd.setCursor(70, 30);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.printf("%d %%\n", analogRead_value);
  M5.Lcd.setCursor(90, 50);
  M5.Lcd.printf("%f C\n", temperature_value);
  
  analogRead_value = 100 - map(analogRead(33), 1300, 4095, 0, 100);
  temperature_value = sensors.getTempCByIndex(0);
    
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setCursor(70, 30);
  M5.Lcd.printf("%d %%\n", analogRead_value);
  M5.Lcd.setCursor(90, 50);
  M5.Lcd.printf("%f C\n", temperature_value);
  
  Blynk.virtualWrite(V1, temperature_value);
  Blynk.virtualWrite(V2, analogRead_value);

  
  if (threshold > analogRead_value && triggered == false)
  {
    triggered = true;
    Blynk.notify("Please water plants !! Moisture levels are lower than the threshold");
  }
  else
  {
    triggered = false;
  }
  delay(100);
}
