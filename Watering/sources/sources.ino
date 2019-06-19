#include <LiquidCrystal.h>
#include "DS3231.h"

// LCD1602
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// DS3231
CDS3231 ds;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  lcd.begin(16, 2);
  lcd.print("hello world!");
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  byte year, month, date, dow, hour, minute, second;
  bool century, h12, PM;
  year = ds.get_year();
  month = ds.get_month(century);
  date = ds.get_date();
  dow = ds.get_dow();
  hour = ds.get_hour(h12, PM);
  minute = ds.get_minute();
  second = ds.get_second();

  //lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(hour < 10 ? "0" + String(hour) : String(hour));
  lcd.print(":");
  lcd.print(minute < 10 ? "0" + String(minute) : String(minute));
  lcd.print(":");
  lcd.print(second < 10 ? "0" + String(second) : String(second));

  Serial.print(hour < 10 ? "0" + String(hour) : String(hour));
  Serial.print(":");
  Serial.print(minute < 10 ? "0" + String(minute) : String(minute));
  Serial.print(":");
  Serial.print(second < 10 ? "0" + String(second) : String(second));

  delay(100);
}
