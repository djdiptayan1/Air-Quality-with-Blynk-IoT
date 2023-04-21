#include <SPI.h>
#include <Wire.h>
#include <DHT.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMonoOblique9pt7b.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET 4      // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BLYNK_TEMPLATE_ID "TMPLoFo97whX"
#define BLYNK_TEMPLATE_NAME "Air Quality"
#define BLYNK_AUTH_TOKEN "2XMs0svi0q-tUNQO-GDT1aabnygjSFSh"
#define BLYNK_PRINT Serial

#define AC 5
#define HVAC 18

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "OPPO K10 5G";  // type your wifi name
char pass[] = "deepak@123";   // type your wifi password

BlynkTimer timer;

#define DHTPIN 13  //Connect Out pin to D2 in NODE MCU
#define gas 33
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float value = 0.00;
float t = 0.0;
float h = 0.0;
String quality = "";

int flag = 0;
int flag2 = 0;

BLYNK_WRITE(V3)  // Executes when the value of virtual pin 0 changes
{
  if (param.asInt() == 1)
    flag = 1;
  else
    flag = 0;
}

BLYNK_WRITE(V4)  // Executes when the value of virtual pin 0 changes
{
  if (param.asInt() == 1)
    flag2 = 1;
  else
    flag2 = 0;
}

void sendSensor() {
  h = dht.readHumidity();
  t = dht.readTemperature();  // or dht.readTemperature(true) for Fahrenheit
  value = analogRead(gas);

  if (t > 40 || flag == 1) {
    Serial.println("TURNING AC ON");
    digitalWrite(AC, HIGH);
  } else
    digitalWrite(AC, LOW);


  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  if (value < 181) {
    quality = "GOOD!";
  } else if (value > 181 && value < 225) {
    quality = "Poor!";
  } else if (value > 225 && value < 300) {
    quality = "Very bad!";
  } else if (value > 300 && value < 350) {
    quality = "ur dead!";
  } else {
    quality = "Toxic";
  }

  if (value > 800 || flag2 == 1) {
    Serial.println("TURNING HVAC ON");
    digitalWrite(HVAC, HIGH);
  } else
    digitalWrite(HVAC, LOW);
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, value);

  Serial.print("Air Quality : ");
  Serial.print(value);

  Serial.print("\tTemperature : ");
  Serial.print(t);

  Serial.print("\tHumidity : ");
  Serial.println(h);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setFont();

  display.setCursor(0, 3);
  display.println("Air Quality :");
  display.setTextSize(1);
  display.setCursor(80, 3);
  display.println(value);
  display.setTextSize(2);
  display.setCursor(16, 18);
  display.println(quality);

  display.setTextSize(1);
  display.setCursor(0, 40);
  display.println("Temperature :");
  display.setTextSize(1);
  display.setCursor(80, 40);
  display.println(t);
  display.setTextSize(1);
  display.setCursor(115, 40);
  display.println("C");

  display.setTextSize(1);
  display.setCursor(0, 54);
  display.println("Humidity    :");
  display.setTextSize(1);
  display.setCursor(80, 54);
  display.println(h);
  display.setTextSize(1);
  display.setCursor(115, 54);
  display.println("%");

  display.display();
}

void setup() {
  Serial.begin(115200);

  pinMode(AC, OUTPUT);
  pinMode(HVAC, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address   0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
  }

  Blynk.begin(auth, ssid, pass);
  pinMode(gas, INPUT);
  dht.begin();
  timer.setInterval(1000L, sendSensor);
}

void loop() {
  Blynk.run();
  timer.run();
}