#include <LiquidCrystal.h>
#include <Keypad.h>
#include "DS3231.h"

// LCD1602
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// DS3231
// 0~24 hours
CDS3231 ds;
byte year, month, date, dow, hour, minute, second;
bool h12, PM;

// KEYS
const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte row_pins[rows] = {2, 3, 4, 5};
byte col_pins[cols] = {6, 7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), row_pins, col_pins, rows, cols);

// State
typedef enum _state
{
  STATE_NORMAL = 0x00,
  STATE_SETTING,
  STATE_DOING
} state_t;
state_t state = STATE_NORMAL;
byte unmasks = 0b00100000;
unsigned long time_ms = 0;
bool flashing = true;

int days_of_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

////////////////////////////////////////////////////////
// Date/Time format(the lowest three bits of mask are effective)
String format_date(byte year, byte month, byte date, byte mask = 0b00000111)
{
  String year_str = "    ";
  if (mask & 0b00000100) {
    year_str = year < 10 ? "200" + String(year) : "20" + String(year);
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
    minute_str = minute < 10 ? "0" + String(minute) : String(minute);
  }
  String second_str = "  ";
  if (mask & 0b00000001) {
    second_str = second < 10 ? "0" + String(second) : String(second);
  }
  return (hour_str + ":" + minute_str + ":" + second_str);
}


////////////////////////////////////////////////////////
void setup() {
  Wire.begin();
  lcd.begin(16, 2);
  lcd.print("hello world!");
  Serial.begin(9600);
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

    if ('1' == keypad.getKey()) {
      unmasks = 0b00100000;
      time_ms = millis();
      state = STATE_SETTING;
      Serial.println("STATE_NORMAL ====> STATE_SETTING");
    }

    delay(200);
  }
  else if (STATE_SETTING == state) {
    char key = keypad.getKey();
    
    switch (key)
    {
    case '1':
      Serial.println("1");
      unmasks = unmasks >> 1;
      if (0b00000001 == unmasks) {
        unmasks = 0b00100000;
      }
      break;
    case '2':
      Serial.println("2");
      switch (unmasks)
      {
      case 0b00100000: // year
        year = year + 1;
        if (year >= 100) {
          year = 0;
        }
        break;
      case 0b00010000: // month
        month = month + 1;
        if (month > 12) {
          month = 1;
        }
        break;
      case 0b00001000: // date
        date = date + 1;
		if (month == 2 && ((0 == year % 100 && 0 == year % 400) || (0 != year % 100 && 0 == year % 4))) {
			if (date > days_of_month[month - 1] + 1) {
				date = 1;
			}
		}
		else {
			if (date > days_of_month[month - 1]) {
				date = 1;
			}
		}
        break;
      case 0b00000100: // hour
		hour = hour + 1;
		if (hour > 23) {
			hour = 0;
		}
        break;
      case 0b00000010: // minute
		minute = minute + 1;
		if (minute > 59) {
			minute = 0;
		}
        break;
      case 0b00000001: // second
		second = second + 1;
		if (second > 59) {
			second = 0;
		}
        break;
      default:
        break;
      }
      break;
    case '3':
      Serial.println("3");
      switch (unmasks)
      {
      case 0b00100000: // year
        year = year - 1;
        if (year <= 0) {
          year = 99;
        }
        break;
      case 0b00010000: // month
        month = month - 1;
        if (month < 1) {
          month = 12;
        }
        break;
      case 0b00001000: // date
		date = date - 1;
		if (date < 1) {
			if (month == 2 && ((0 == year % 100 && 0 == year % 400) || (0 != year % 100 && 0 == year % 4))) {
				date = days_of_month[month - 1] + 1;
			}
			else {
				date = days_of_month[month - 1];
			}
		}
        break;
      case 0b00000100: // hour
		hour = hour - 1;
		if (hour < 0) {
			hour = 23;
		}
        break;
      case 0b00000010: // minute
		minute = minute - 1;
		if (minute < 0) {
			minute = 59;
		}
        break;
      case 0b00000001: // second
		second = second - 1;
		if (second < 0) {
			second = 59;
		}
        break;
      default:
        break;
      }
      break;
    case 'A':
      Serial.println("A");
	  ds.set_year(year);
	  ds.set_month(month);
	  ds.set_date(date);
	  ds.set_hour(hour);
	  ds.set_minute(minute);
	  ds.set_second(second);
	  state = STATE_NORMAL;
      break;
    default:
      break;
    }

    byte masks = flashing ? 0b00000111 : (((~unmasks) >> 3) & 0b00000111);
    Serial.print(format_date(year, month, date, masks));
    Serial.print(" ");
    masks = flashing ? 0b00000111 : ((~unmasks) & 0b00000111);
    Serial.print(format_time(hour, minute, second, masks));
    Serial.println();

    if (millis() -  time_ms >= 500) {
      flashing = !flashing;
      time_ms = millis();
    }

    //delay(200);
  }
}
