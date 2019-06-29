#include <LiquidCrystal.h>
#include <Keypad.h>
#include "DS3231.h"

// LCD1602
LiquidCrystal lcd(9, 8, 4, 5, 6, 7);

// DS3231
// 0~24 hours
CDS3231 ds;
byte year, month, date, week, hour, minute, second;
bool dy, h12, PM, enable;
byte alarm_bits;

// KEYS
const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};
byte row_pins[rows] = {10, 11, 12, A1};
byte col_pins[cols] = {A0, 13, A2, A3};
Keypad keypad = Keypad(makeKeymap(keys), row_pins, col_pins, rows, cols);

// State
typedef enum _state
{
	STATE_NORMAL = 0x00,
	STATE_SET_TIME,
	STATE_SET_ALARM1,
	STATE_SET_ALARM2,
	STATE_DOING
} state_t;
state_t state = STATE_NORMAL;
byte unmasks = 0b01000000;
unsigned long time_ms = 0;
bool flashing = true;

int days_of_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
String days_of_week[] = { "Mon", "Tues", "Wed", "Thur", "Fri", "Sat", "Sun" };


////////////////////////////////////////////////////////
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
String format_week(byte week, byte mask = 0b00000001)
{
	if (mask & 0b00000001) {
		return days_of_week[week];
	}
	else {
		return "    ";
	}
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
////////////////////////////////////////////////////////
void setup() {
	Wire.begin();
	lcd.begin(16, 2);
	lcd.print("");
	//Serial.begin(9600);
}

////////////////////////////////////////////////////////
void loop() {
	if (STATE_NORMAL == state) {
		ds.get_time(year, month, date, week, h12, PM, hour, minute, second);
		if (h12 && PM) {
			hour = hour + 12;
		}

		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(format_date(year, month, date));
		lcd.print(" ");
		lcd.print(format_week(week - 1));
		lcd.setCursor(0, 1);
		lcd.print(format_time(hour, minute, second));

		//Serial.print(format_date(year, month, date));
		//Serial.print(" ");
		//Serial.print(format_week(week - 1));
		//Serial.print(" ");
		//Serial.print(format_time(hour, minute, second));
		//Serial.println();

		char key = keypad.getKey();
		if ('1' == key) {
			unmasks = 0b01000000;
			time_ms = millis();
			
			state = STATE_SET_TIME;
			//Serial.println("STATE_NORMAL ====> STATE_SET_TIME");
		}
		else if ('4' == key) {
			unmasks = 0b00001000;
			time_ms = millis();

			enable = ds.is_alarm_enabled(1);
			ds.get_alarm1(date, hour, minute, second, alarm_bits, dy, h12, PM);
			
			state = STATE_SET_ALARM1;
			//Serial.println("STATE_NORMAL ====> STATE_SET_ALARM1");
		}
		else if ('7' == key) {
			unmasks = 0b00000100;
			time_ms = millis();

			enable = ds.is_alarm_enabled(2);
			ds.get_alarm2(date, hour, minute, alarm_bits, dy, h12, PM);

			state = STATE_SET_ALARM2;
			//Serial.println("STATE_NORMAL ====> STATE_SET_ALARM2");
		}

		if (ds.is_alarming(1, true)) {
			//Serial.println("=====> ALARM1");
		}
		if (ds.is_alarming(2, true)) {
			//Serial.println("=====> ALARM2");
		}

		delay(200);
	}
	else if (STATE_SET_TIME == state) {
		char key = keypad.getKey();
    
		switch (key)
		{
		case '1':
			if (0b00000001 == unmasks) {
				unmasks = 0b01000000;
			}
			else {
				unmasks = unmasks >> 1;
			}
			break;
		case '2':
			switch (unmasks)
			{
			case 0b01000000: // year
				year = year + 1;
				if (year >= 100) {
					year = 0;
				}
				break;
			case 0b00100000: // month
				month = month + 1;
				if (month > 12) {
					month = 1;
				}
				break;
			case 0b00010000: // date
				date = date + 1;
				if (month == 2 && ((0 == year % 100 && 0 == year % 400) || (0 != year % 100 && 0 == year % 4))) {
					if (date > days_of_month[month - 1] + 1) {
						date = 1;
					}
				}
				else if (date > days_of_month[month - 1]) {
					date = 1;
				}
				break;
			case 0b00001000: // week
				week = week + 1;
				if (week > 7) {
					week = 1;
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
			switch (unmasks)
			{
			case 0b01000000: // year
				if (0 == year) {
					year = 99;
				}
				else {
					year = year - 1;
				}
				break;
			case 0b00100000: // month
				if (1 == month) {
					month = 12;
				}
				else {
					month = month - 1;
				}
				break;
			case 0b00010000: // date
				if (1 == date) {
					if (month == 2 && ((0 == year % 100 && 0 == year % 400) || (0 != year % 100 && 0 == year % 4))) {
						date = days_of_month[month - 1] + 1;
					}
					else {
						date = days_of_month[month - 1];
					}
				}
				else {
					date = date - 1;
				}
				break;
			case 0b00001000: // week
				if (1 == week) {
					week = 7;
				}
				else {
					week = week - 1;
				}
				break;
			case 0b00000100: // hour
				if (0 == hour) {
					hour = 23;
				}
				else {
					hour = hour - 1;
				}
				break;
			case 0b00000010: // minute
				if (0 == minute) {
					minute = 59;
				}
				else {
					minute = minute - 1;
				}
				break;
			case 0b00000001: // second
				if (0 == second) {
					second = 59;
				}
				else {
					second = second - 1;
				}
				break;
			default:
				break;
			}
			break;
		case 'A':
			ds.set_year(year);
			ds.set_month(month);
			ds.set_date(date);
			ds.set_week(week);
			ds.set_hour(hour, false);
			ds.set_minute(minute);
			ds.set_second(second);
			state = STATE_NORMAL;
			break;
		case 'D':
			state = STATE_NORMAL;
			break;
		default:
			break;
		}

		byte masks = flashing ? 0b00000111 : (((~unmasks) >> 4) & 0b00000111);
		lcd.setCursor(0, 0);
		lcd.print(format_date(year, month, date, masks));
		lcd.print(" ");
		//Serial.print(format_date(year, month, date, masks));
		//Serial.print(" ");
		masks = flashing ? 0b00000001 : (((~unmasks) >> 3) & 0b00000001);
		lcd.print(format_week(week - 1, masks));
		//Serial.print(format_week(week - 1, masks));
		//Serial.print(" ");
		masks = flashing ? 0b00000111 : ((~unmasks) & 0b00000111);
		lcd.setCursor(0, 1);
		lcd.print(format_time(hour, minute, second, masks));
		//Serial.print(format_time(hour, minute, second, masks));
		//Serial.println();

		if (millis() -  time_ms >= 500) {
			flashing = !flashing;
			time_ms = millis();
		}

		// delay(200);
	}
	else if (STATE_SET_ALARM1 == state) {
		String str = enable ? "enable" : "disable";
		lcd.println("ALARM1: " + String(hour) + ":" + String(minute) + ":" + String(second) + " " + str);
		//Serial.println("ALARM1: " + String(hour) + ":" + String(minute) + ":" + String(second) + " " + str);

		char key = keypad.getKey();
		switch (key)
		{
		case '4':
			if (0b00000001 == unmasks) {
				unmasks = 0b00001000;
			}
			else {
				unmasks = unmasks >> 1;
			}
			break;
		case '5':
			switch (unmasks)
			{
			case 0b00001000: // hour
				hour = hour + 1;
				if (hour > 23) {
					hour = 0;
				}
				break;
			case 0b00000100: // minute
				minute = minute + 1;
				if (minute > 59) {
					minute = 0;
				}
				break;
			case 0b00000010: // second
				second = second + 1;
				if (second > 59) {
					second = 0;
				}
				break;
			case 0b00000001: // enable
				enable = !enable;
				break;
			default:
				break;
			}
			break;
		case '6':
			switch (unmasks)
			{
			case 0b00001000: // hour
				if (0 == hour) {
					hour = 23;
				}
				else {
					hour = hour - 1;
				}
				break;
			case 0b00000100: // minute
				if (0 == minute) {
					minute = 59;
				}
				else {
					minute = minute - 1;
				}
				break;
			case 0b00000010: // second
				if (0 == second) {
					second = 59;
				}
				else {
					second = second - 1;
				}
				break;
			case 0b00000001: // enable
				enable = !enable;
				break;
			default:
				break;
			}
			break;
		case 'B':
			alarm_bits = 0b00001000;
			ds.set_alarm1(date, hour, minute, second, alarm_bits, false, false);
			if (enable) {
				ds.turn_on_alarm(1);
			}
			else {
				ds.turn_off_alarm(1);
			}
			state = STATE_NORMAL;
			break;
		case 'D':
			state = STATE_NORMAL;
			break;
		default:
			break;
		}

		// delay(200);
	}
	else if (STATE_SET_ALARM2 == state) {
		String str = enable ? "enable" : "disable";
		lcd.println("ALARM2: " + String(hour) + ":" + String(minute) + " " + str);
		//Serial.println("ALARM2: " + String(hour) + ":" + String(minute) + " " + str);

		char key = keypad.getKey();
		switch (key)
		{
		case '7':
			if (0b00000001 == unmasks) {
				unmasks = 0b00000100;
			}
			else {
				unmasks = unmasks >> 1;
			}
			break;
		case '8':
			switch (unmasks)
			{
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
			case 0b00000001: // enable
				enable = !enable;
				break;
			default:
				break;
			}
			break;
		case '9':
			switch (unmasks)
			{
			case 0b00000100: // hour
				if (0 == hour) {
					hour = 23;
				}
				else {
					hour = hour - 1;
				}
				break;
			case 0b00000010: // minute
				if (0 == minute) {
					minute = 59;
				}
				else {
					minute = minute - 1;
				}
				break;
			case 0b00000001: // enable
				enable = !enable;
				break;
			default:
				break;
			}
			break;
		case 'C':
			alarm_bits = 0b00000100;
			ds.set_alarm2(date, hour, minute, alarm_bits, false, false);
			if (enable) {
				ds.turn_on_alarm(2);
			}
			else {
				ds.turn_off_alarm(2);
			}
			state = STATE_NORMAL;
			break;
		case 'D':
			state = STATE_NORMAL;
			break;
		default:
			break;
		}

		// delay(200);
	}
	else if (STATE_DOING == state) {
		// TODO
		// ...
		
		// delay(200);
	}
}
