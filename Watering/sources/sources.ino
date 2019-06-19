#include <LiquidCrystal.h>
#include "DS3231.h"

// LCD1602
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// DS3231
CDS3231 ds;

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
  STATE_SETTING
} state_t;
state_t state;

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

void setup() {
  Wire.begin();
  lcd.begin(16, 2);
  lcd.print("hello world!");
  Serial.begin(9600);

  pinMode(KEY_MODE_PIN, INPUT_PULLUP);
  pinMode(KEY_PLUS_PIN, INPUT_PULLUP);
  pinMode(KEY_MINUS_PIN, INPUT_PULLUP);
  pinMode(KEY_OK_PIN, INPUT_PULLUP);

  state = STATE_NORMAL;
}

void loop() {
  if (STATE_NORMAL == state) {
    byte year, month, date, dow, hour, minute, second;
    bool h12, PM;
    ds.get_time(year, month, date, dow, h12, PM, hour, minute, second);

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
    Serial.println();

    if (KEY_MODE == scan()) {
      state = STATE_SETTING;
      Serial.println("====>");
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
  }
}
