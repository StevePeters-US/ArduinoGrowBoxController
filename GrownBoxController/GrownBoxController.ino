
#include <LiquidCrystal_I2C.h>//Marco Shwartz
#include <DHT.h> // DHT Sensor Library - Adafruit
#include <Wire.h>
#include <DS3231.h> //http://www.rinkydinkelectronics.com/library.php?id=73

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

#define DHTPIN 12     // Temp / Humidity sensor pin
#define DHTTYPE DHT11   // DHT 11 

const int fanRelay = 8;
const int lightRelay = 11;
#define LIGHTSWITCH 10
bool lightSwitchPressed = false;

LiquidCrystal_I2C lcd(0x27, 20, 4);

bool lightOn = false;
int lightOnHour = 6;
int lightOnMin = 00;

int lightOffHour = 20;
int lightOffMin = 18;

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  lcd.init();  //initialize lcd screen
  lcd.backlight();  // turn on the backlight

  lcd.setCursor(0, 1);
  lcd.print("Initializing");

  Serial.begin(9600);
  dht.begin();

  // Start the I2C interface
  Wire.begin();

  pinMode(fanRelay, OUTPUT);
  digitalWrite(fanRelay, HIGH);

  pinMode(lightRelay, OUTPUT);
  //digitalWrite(lightRelay, HIGH);
  pinMode(LIGHTSWITCH, INPUT_PULLUP);

  // Initialize the real time clock object
  rtc.begin();

  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(THURSDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(18, 11, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(9, 2, 2021);   // Set the date to January 1st, 2014

  delay(3000);

  if (false) {
    SetDebugLightTime();
  }

}

void loop() {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in t in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");

  lcd.setCursor(0, 0);
  lcd.print("temp: ");
  lcd.print(t);   // Printing terature on LCD
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(h);
  lcd.setCursor(0, 2);
  lcd.print(rtc.getTimeStr());

  if (h > 60)
  {
    // Fan on
    digitalWrite(fanRelay, HIGH);
  }

  else if (h < 55)
  {
    //Fan Off
    digitalWrite(fanRelay, LOW);
  }

  CheckLightTime();

  int sensorVal = 1 - digitalRead(LIGHTSWITCH);
  if (!lightSwitchPressed && sensorVal == 1)
  {
    lightSwitchPressed = true;
    ToggleLight();
  }

  else if (sensorVal == 0) {
    lightSwitchPressed = false;
  }
  //print out the value of the pushbutton

  Serial.println(sensorVal);

  delay(1000);
}

void SetDebugLightTime()
{
  Time t = rtc.getTime();
  //Debug light timer
  lightOnHour = t.hour;
  lightOnMin = t.min + 1;

  lightOffHour = t.hour;;
  lightOffMin = t.min + 2;
}

void ToggleLight()
{
  if (lightOn == false)
  {
    TurnLightOn();
  }

  else if (lightOn == true)
  {
    TurnLightOff();
  }
}

void TurnLightOn()
{
  lightOn = true;
  digitalWrite(lightRelay, HIGH);
  Serial.print("LIGHT ON!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
}

void TurnLightOff()
{
  lightOn = false;
  digitalWrite(lightRelay, LOW);
  Serial.print("LIGHT OFF!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
}

void CheckLightTime()
{

  Time t = rtc.getTime();
  Serial.print(t.hour);
  Serial.print(" : ");
  Serial.println(t.min);

  if (lightOn == false && t.hour == lightOnHour && t.min == lightOnMin)
  {
    TurnLightOn();
  }

  else if (lightOn == true && t.hour == lightOffHour && t.min == lightOffMin)
  {
    TurnLightOff();
  }

}
