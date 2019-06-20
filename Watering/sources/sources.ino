#include <LiquidCrystal.h>
#include "DS3231.h"

// LCD1602
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// DS3231
CDS3231 ds;
byte year, month, date, dow, hour, minute, second;
bool h12, PM;

// KEYS
#define KEY_MODE_PIN    2
#define KEY_PLUS_PIN    3
#define KEY_MINUS_PIN   4
#define KEY_OK_PIN      5
typedef enum _key
{
  KEY_NONE = 0x00,
  KEY_MODE,
  KEY_PLUS,
  KEY_MINUS,
  KEY_OK
} key_t;
int last_mode = HIGH;
int last_plus = HIGH;
int last_minus = HIGH;
int last_ok = HIGH;

// State
typedef enum _state
{
  STATE_NORMAL = 0x00,
  STATE_SETTING,
  STATE_DOING
} state_t;
state_t state = STATE_NORMAL;
byte unmasks = 0b00100000;

////////////////////////////////////////////////////////
// Scan keys
int scan()
{
  int value = KEY_NONE;
  
  int cur_mode = digitalRead(KEY_MODE_PIN);
  int cur_plus = digitalRead(KEY_PLUS_PIN);
  int cur_minus = digitalRead(KEY_MINUS_PIN);
  int cur_ok = digitalRead(KEY_OK_PIN);
  if (cur_mode != last_mode) {
    delay(20);
    cur_mode = digitalRead(KEY_MODE_PIN);
    if (HIGH == last_mode && LOW == cur_mode) {
      value = KEY_MODE;
    }
  }
  else if (cur_plus != last_plus) {
    delay(20);
    cur_plus = digitalRead(KEY_PLUS_PIN);
    if (HIGH == last_plus && LOW == cur_plus) {
      value = KEY_PLUS;
    }
  }
  else if (cur_minus != last_minus) {
    delay(20);
    cur_minus = digitalRead(KEY_MINUS_PIN);
    if (HIGH == last_minus && LOW == cur_minus) {
      value = KEY_MINUS;
    }
  }
  else if (cur_ok != last_ok) {
    delay(20);
    cur_ok = digitalRead(KEY_OK_PIN);
    if (HIGH == last_ok && LOW == cur_ok) {
      value = KEY_OK;
    }
  }
  last_mode = cur_mode;
  last_plus = cur_plus;
  last_minus = cur_minus;
  last_ok = cur_ok;

  return value;
}

////////////////////////////////////////////////////////
// Date/Time format(the lowest three bits of mask are effective)
String format_date(byte year, byte month, byte date, byte mask = 0b00000111)
{
  String year_str = "    ";
  if (mask & 0b00000100) {
    year_str = "20" + String(year);
  }
  String month_str = "  ";
  if (mask & 0b00000010) {
    month_str = month < 10 ? "0" + String(month) : String(month);
  }
  String date_str = "  ";
  if (mask & 0b00000001) {
    date_str = date < 10 ? "0" + String(date) : String(date);
  }
  return (year_str + "-" + month_str + "-" + date_str);
}
String format_time(byte hour, byte minute, byte second, byte mask = 0b00000111)
{
  String hour_str = "  ";
  if (mask & 0b00000100) {
    hour_str = hour < 10 ? "0" + String(hour) : String(hour);
  }
  String minute_str = "  ";
  if (mask & 0b00000010) {
    minute_str = minute < 10 ? "0" + String(minute_str) : String(minute_str);
  }
  String second_str = "  ";
  if (mask & 0b00000001) {
    second_str = second < 10 ? "0" + String(second_str) : String(second_str);
  }
  return (hour_str + ":" + minute_str + ":" + second_str);
}


////////////////////////////////////////////////////////
void setup() {
  Wire.begin();
  lcd.begin(16, 2);
  lcd.print("hello world!");
  Serial.begin(9600);

  pinMode(KEY_MODE_PIN, INPUT_PULLUP);
  pinMode(KEY_PLUS_PIN, INPUT_PULLUP);
  pinMode(KEY_MINUS_PIN, INPUT_PULLUP);
  pinMode(KEY_OK_PIN, INPUT_PULLUP);
}

////////////////////////////////////////////////////////
void loop() {
  if (STATE_NORMAL == state) {
    ds.get_time(year, month, date, dow, h12, PM, hour, minute, second);
    if (h12 && PM) {
      hour = hour + 12;
    }

    //lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(format_date(year, month, date));
    lcd.setCursor(1, 1);
    lcd.print(format_time(hour, minute, second));

    Serial.print(format_date(year, month, date));
    Serial.print(" ");
    Serial.print(format_time(hour, minute, second));
    Serial.println();

    if (KEY_MODE == scan()) {
      state = STATE_SETTING;
      Serial.println("STATE_NORMAL ====> STATE_SETTING");
    }

    delay(200);
  }
  else if (STATE_SETTING == state) {
    int key = scan();

    switch (key)
    {
    case KEY_NONE:
      break;
    case KEY_MODE:
      Serial.println("KEY_MODE");
      unmasks = unmasks >> 1;
      if (0b00000001 == unmasks) {
        unmasks = 0b00100000;
      }
      break;
    case KEY_PLUS:
      Serial.println("KEY_PLUS");
      break;
    case KEY_MINUS:
      Serial.println("KEY_MINUS");
      break;
    case KEY_OK:
      Serial.println("KEY_OK");
      break;
    default:
      break;
    }

    byte masks = ((~unmasks) >> 3) & 0b00000111;
    Serial.print(format_date(year, month, date, masks));
    Serial.print(" ");
    masks = (~unmasks) & 0b00000111;
    Serial.print(format_time(hour, minute, second, masks));
    Serial.println();
  }
}
