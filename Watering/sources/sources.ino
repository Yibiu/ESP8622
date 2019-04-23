#include <Wire.h>
#include <LiquidCrystal.h>
#include "DS3231.h"

CDS3231 ds;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  lcd.begin(16, 2);
  lcd.print("hello world!");
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
  if (hour < 10)
    lcd.print("0");
  lcd.print(String(hour));
  lcd.print(":");
  if (minute < 10)
    lcd.print("0");
  lcd.print(String(minute));
  lcd.print(":");
  if (second < 10)
    lcd.print("0");
  lcd.print(String(second));
}

