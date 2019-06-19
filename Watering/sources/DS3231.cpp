#include "DS3231.h"


CDS3231::CDS3231()
{
}

CDS3231::~CDS3231()
{
}

void CDS3231::get_time(byte &year, byte &month, byte &date, byte &dow,
    bool &h12, bool &PM, byte &hour, byte &minute, byte &second)
{
  Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x00));
	Wire.endTransmission();
	
	Wire.requestFrom(CLOCK_ADDRESS, 7);
	second = _bcd_to_dec(Wire.read());              // 00H
	minute = _bcd_to_dec(Wire.read());              // 01H
	byte tmp = Wire.read();                         // 02H
	h12 = tmp & 0b01000000;
	if (h12) {
		PM = tmp & 0b00100000;
		hour = _bcd_to_dec(tmp & 0b00011111);
	}
	else {
	  hour = _bcd_to_dec(tmp & 0b00111111);
	}
	dow = _bcd_to_dec(Wire.read());                 // 03H
	date = _bcd_to_dec(Wire.read());                // 04H
	month = _bcd_to_dec(Wire.read() & 0b01111111);  // 05H
	year = _bcd_to_dec(Wire.read());                // 06H
}

byte CDS3231::get_year()
{
  Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x06));                      // 06H
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	return _bcd_to_dec(Wire.read());
}

byte CDS3231::get_month(bool &century)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x05));                     // 05H
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	byte month = Wire.read();
	century = month & 0b10000000;
	return (_bcd_to_dec(month & 0b01111111));
}

byte CDS3231::get_date()
{
  Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x04));                     // 04H
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	return _bcd_to_dec(Wire.read());
}

byte CDS3231::get_dow()
{
  Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x03));                     // 03H
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	return _bcd_to_dec(Wire.read());
}

byte CDS3231::get_hour(bool &h12, bool &PM)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(0x02);                              // 02H
	Wire.endTransmission();

  byte hour = 0;
	Wire.requestFrom(CLOCK_ADDRESS, 1);
	byte tmp = Wire.read();
	h12 = tmp & 0b01000000;
	if (h12) {
	  PM = tmp & 0b00100000;
		hour = _bcd_to_dec(tmp & 0b00011111);
	}
	else {
		hour = _bcd_to_dec(tmp & 0b00111111);
	}
	return hour;
}

byte CDS3231::get_minute()
{
  Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(0x01);                             // 01H
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	return _bcd_to_dec(Wire.read());
}

byte CDS3231::get_second()
{
  Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x00));                    // 00H
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	return _bcd_to_dec(Wire.read());
}

void CDS3231::set_year(byte year)
{
  Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x06));                     // 06H
	Wire.write(_dec_to_bcd(year));
	Wire.endTransmission();
}

void CDS3231::set_month(byte month)
{
  Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x05));                     // 05H
	Wire.write(_dec_to_bcd(month));
	Wire.endTransmission();
}

void CDS3231::set_date(byte date)
{
  Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x04));                     // 04H
	Wire.write(_dec_to_bcd(date));
	Wire.endTransmission();
}

void CDS3231::set_dow(byte dow)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x03));                // 03H
	Wire.write(_dec_to_bcd(dow));	
	Wire.endTransmission();
}

void CDS3231::set_hour(byte hour)
{
  Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x02));                // 02H
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	bool h12 = (Wire.read() & 0b01000000);
	if (h12) {
		if (hour >= 12) {
			hour = _dec_to_bcd(hour - 12) | 0b01100000;
		}
		else {
			hour = _dec_to_bcd(hour) & 0b11011111;
		}
	}
	else {
		hour = _dec_to_bcd(hour) & 0b10111111;
	}

	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x02));
	Wire.write(hour);
	Wire.endTransmission();
}

void CDS3231::set_hour_mode(bool h12)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x02));                 // 02H
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	byte tmp = Wire.read();
  bool cur_h12 = tmp & 0b01000000;
  
  // No need to change
  if ((cur_h12 && h12) || ((!cur_h12) && (!h12)))
    return;

  byte hour = 0;
  // 12 --> 24
  if (cur_h12) {
    bool PM = tmp & 0b00100000;
    hour = _bcd_to_dec(tmp & 0b00011111);
    if (PM) {
      hour = hour + 12;
    }
    hour = _dec_to_bcd(hour);
    hour = hour & 0b00111111;
  }
  // 24 --> 12
  else {
    hour = _bcd_to_dec(tmp & 0b00111111);
    if (hour >= 12) {
      hour = hour - 12;
      hour = _dec_to_bcd(hour);
      hour = hour | 0b01100000;
    }
    else {
      hour = _dec_to_bcd(hour);
      hour = hour & 0b11011111;
      hour = hour | 0b01000000;
    }
  }

	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x02));
	Wire.write(hour);
	Wire.endTransmission();
}

void CDS3231::set_minute(byte minute)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x01));                 // 01H
	Wire.write(_dec_to_bcd(minute));
	Wire.endTransmission();
}

void CDS3231::set_second(byte second)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x00));                // 00H
	Wire.write(_dec_to_bcd(second));
	Wire.endTransmission();

	// Clear OSF flag
	byte tmp = _read_control_byte(1);
	_write_control_byte(1, (tmp & 0b01111111));
}

float CDS3231::get_temperature()
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x11));               // 11H~12H
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 2);
	byte temp_msb = Wire.read();
	return float(temp_msb) + 0.25 * (Wire.read() >> 6);
}

void CDS3231::get_alarm1(byte &day, byte &hour, byte &minute, byte &second,
  byte &alarm_bits, bool &dy, bool &h12, bool &PM)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x07));
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 4);
	byte tmp = Wire.read();                                // 07H
	second = _bcd_to_dec(tmp & 0b01111111);
	alarm_bits = (tmp & 0b10000000) >> 7;
	tmp = Wire.read();
	minute = _bcd_to_dec(tmp & 0b01111111);               // 08H
	alarm_bits = alarm_bits | ((tmp & 0b10000000) >> 6);
	tmp = Wire.read();                                    // 09H
	h12 = tmp & 0b01000000;
	if (h12) {
		PM	= tmp & 0b00100000;
		hour = _bcd_to_dec(tmp & 0b00011111);
	}
	else {
		hour = _bcd_to_dec(tmp & 0b00111111);
	}
	alarm_bits = alarm_bits | ((tmp & 0b10000000) >> 5);
	tmp = Wire.read();                                   // 0AH
	dy = (tmp & 0b01000000);
	if (dy) {
		day	= _bcd_to_dec(tmp & 0b00001111);
	}
	else {
		day	= _bcd_to_dec(tmp & 0b00111111);
	}
	alarm_bits = alarm_bits | ((tmp & 0b10000000) >> 4);
}

void CDS3231::get_alarm2(byte &day, byte &hour, byte &minute,
  byte &alarm_bits, bool &dy, bool &h12, bool &PM)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x0b));
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 3);
	byte tmp = Wire.read();                            // 0BH
	minute = _bcd_to_dec(tmp & 0b01111111);
	alarm_bits = (tmp & 0b10000000) >> 7;             
	tmp = Wire.read();                                // 0CH
	h12 = tmp & 0b01000000;
	if (h12) {
		PM = tmp & 0b00100000;
		hour = _bcd_to_dec(tmp & 0b00011111);
	}
	else {
		hour = _bcd_to_dec(tmp & 0b00111111);
	}
  alarm_bits = alarm_bits | ((tmp & 0b10000000) >> 6);
	tmp = Wire.read();                                // 0DH
	dy = tmp & 0b01000000;
	if (dy) {
		day	= _bcd_to_dec(tmp & 0b00001111);
	}
	else {
		day = _bcd_to_dec(tmp & 0b00111111);
	}
	alarm_bits = alarm_bits | ((tmp & 0b10000000) >> 5);
}

void CDS3231::set_alarm1(byte day, byte hour, byte minute, byte second, byte alarm_bits, bool dy, bool h12)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x07));
	Wire.write(_dec_to_bcd(second) | ((alarm_bits & 0b00000001) << 7));     // 07H
	Wire.write(_dec_to_bcd(minute) | ((alarm_bits & 0b00000010) << 6));     // 08H
  byte tmp;
  bool PM = false;
	if (h12) {
		if (hour > 12) {
			hour = hour - 12;
			PM = true;
		}
		if (PM) {
			tmp = _dec_to_bcd(hour) | 0b01100000;
		}
		else {
			tmp = _dec_to_bcd(hour) | 0b01000000;
		}
	}
	else {
		tmp = _dec_to_bcd(hour);
	}
	tmp = tmp | ((alarm_bits & 0b00000100) << 5);
	Wire.write(tmp);                                                      // 09H
	tmp = ((alarm_bits & 0b00001000) << 4) | _dec_to_bcd(day);
	if (dy) {
		tmp = tmp | 0b01000000;
	}
	Wire.write(tmp);                                                      // 0AH
	Wire.endTransmission();
}

void CDS3231::set_alarm2(byte day, byte hour, byte minute, byte alarm_bits, bool dy, bool h12)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x0b));
	Wire.write(_dec_to_bcd(minute) | ((alarm_bits & 0b00000001) << 7));   // 0BH
  byte tmp;
  bool PM = false;
  if (h12) {
		if (hour > 12) {
			hour = hour - 12;
			PM = true;
		}
		if (PM) {
			tmp = _dec_to_bcd(hour) | 0b01100000;
		}
    else {
			tmp = _dec_to_bcd(hour) | 0b01000000;
		}
	}
  else {
		tmp = _dec_to_bcd(hour);
	}
	tmp = tmp | ((alarm_bits & 0b00000010) << 6);
	Wire.write(tmp);                                                   // 0CH
	tmp = ((alarm_bits & 0b00000100) << 5) | _dec_to_bcd(day);
	if (dy) {
		tmp = tmp | 0b01000000;
	}
	Wire.write(tmp);                                                    // 0DH
	Wire.endTransmission();
}

void CDS3231::turn_on_alarm(byte alarm)
{
	byte tmp = _read_control_byte(0);
	if (1 == alarm) {
	  tmp = tmp | 0b00000101;
  }
  else {
    tmp = tmp | 0b00000110;
  }
	_write_control_byte(0, tmp);
}

void CDS3231::turn_off_alarm(byte alarm)
{
	byte tmp = _read_control_byte(0);
	if (1 == alarm) {
	  tmp = tmp & 0b11111110;
  }
  else {
    tmp = tmp & 0b11111101;
  }
	_write_control_byte(0, tmp);
}

bool CDS3231::is_alarm_enabled(byte alarm)
{
  byte result = 0x00;
	byte tmp = _read_control_byte(0);
	if (1 == alarm) {
		result = tmp & 0b00000001;
	}
	else {
		result = tmp & 0b00000010;
	}
	return result;
}

bool CDS3231::is_alarming(byte alarm)
{
	byte result = 0x00;
	byte tmp = _read_control_byte(1);
	if (1 == alarm) {
		result = tmp & 0b00000001;
		tmp = tmp & 0b11111110;
	}
  else {
		result = tmp & 0b00000010;
		tmp = tmp & 0b11111101;
	}
	_write_control_byte(1, tmp);
	return result;
}

void CDS3231::enable_oscillator(bool TF, bool battery, byte frequency)
{
  // turns oscillator on or off. True is on, false is off.
	// if battery is true, turns on even for battery-only operation,
	// otherwise turns off if Vcc is off.
	// frequency must be 0, 1, 2, or 3.
	// 0 = 1 Hz
	// 1 = 1.024 kHz
	// 2 = 4.096 kHz
	// 3 = 8.192 kHz (Default if frequency byte is out of range)
	if (frequency > 3) frequency = 3;
	// read control byte in, but zero out current state of RS2 and RS1.
	byte temp = _read_control_byte(0) & 0b11100111;
	if (battery) {
		// turn on BBSQW flag
		temp = temp | 0b01000000;
	} else {
		// turn off BBSQW flag
		temp = temp & 0b10111111;
	}
	if (TF) {
		// set ~EOSC to 0 and INTCN to zero.
		temp = temp & 0b01111011;
	} else {
		// set ~EOSC to 1, leave INTCN as is.
		temp = temp | 0b10000000;
	}
	// shift frequency into bits 3 and 4 and set.
	frequency = frequency << 3;
	temp = temp | frequency;
	// And write the control bits
	_write_control_byte(temp, 0);
}

void CDS3231::enable_32kHz_output(bool TF)
{
  byte temp = _read_control_byte(1);
	if (TF) {
		// turn on 32kHz pin
		temp = temp | 0b00001000;
	} else {
		// turn off 32kHz pin
		temp = temp & 0b11110111;
	}
	_write_control_byte(temp, 1);
}

bool CDS3231::is_oscillator_running()
{
  byte temp = _read_control_byte(1);
	bool result = true;
	if (temp & 0b10000000) {
		// Oscillator Stop Flag (OSF) is set, so return false.
		result = false;
	}
	return result;
}

byte CDS3231::_dec_to_bcd(byte val)
{
  return ((val / 10 * 16) + (val % 10));
}

byte CDS3231::_bcd_to_dec(byte val)
{
  return ((val / 16 * 10) + (val % 16));
}

byte CDS3231::_read_control_byte(bool which)
{
  Wire.beginTransmission(CLOCK_ADDRESS);
  if (which) {
    // Second control byte
    Wire.write(uint8_t(0x0f));
  }
  else {
    // First control byte
    Wire.write(uint8_t(0x0e));
  }
  Wire.endTransmission();

  Wire.requestFrom(CLOCK_ADDRESS, 1);
  return Wire.read();	
}

void CDS3231::_write_control_byte(bool which, byte val)
{
  Wire.beginTransmission(CLOCK_ADDRESS);
  if (which) {
    Wire.write(uint8_t(0x0f));
  }
  else {
    Wire.write(uint8_t(0x0e));
  }
  Wire.write(val);
  Wire.endTransmission();
}
