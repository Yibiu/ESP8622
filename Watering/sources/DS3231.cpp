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
	second = _bcd_to_dec(Wire.read());
	minute = _bcd_to_dec(Wire.read());
	hour = _bcd_to_dec(Wire.read());
	h12 = hour & 0b01000000;
	if (h12) {
		PM = hour & 0b00100000;
		hour = _bcd_to_dec(hour & 0b00011111);
	}
    else {
		hour = _bcd_to_dec(hour & 0b00111111);
	}
	dow = _bcd_to_dec(Wire.read());
	date = _bcd_to_dec(Wire.read());
	month = _bcd_to_dec(Wire.read() & 0b01111111);
	year = _bcd_to_dec(Wire.read());
}

byte CDS3231::get_year()
{
    Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x06));
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	return _bcd_to_dec(Wire.read());
}

byte CDS3231::get_month(bool &century)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x05));
	Wire.endTransmission();

    byte month;
	Wire.requestFrom(CLOCK_ADDRESS, 1);
	month = Wire.read();
	century = month & 0b10000000;
	return (_bcd_to_dec(month & 0b01111111));
}

byte CDS3231::get_date()
{
    Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x04));
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	return _bcd_to_dec(Wire.read());
}

byte CDS3231::get_dow()
{
    Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x03));
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	return _bcd_to_dec(Wire.read());
}

byte CDS3231::get_hour(bool &h12, bool &PM)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(0x02);
	Wire.endTransmission();

    byte temp, hour;
	Wire.requestFrom(CLOCK_ADDRESS, 1);
	temp = Wire.read();
	h12 = temp & 0b01000000;
	if (h12) {
		PM = temp & 0b00100000;
		hour = _bcd_to_dec(temp & 0b00011111);
	}
    else {
		hour = _bcd_to_dec(temp & 0b00111111);
	}
	return hour;
}

byte CDS3231::get_minute()
{
    Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(0x01);
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	return _bcd_to_dec(Wire.read());
}

byte CDS3231::get_second()
{
    Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x00));
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	return _bcd_to_dec(Wire.read());
}

void CDS3231::set_year(byte year)
{
    Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x06));
	Wire.write(_dec_to_bcd(year));
	Wire.endTransmission();
}

void CDS3231::set_month(byte month)
{
    Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x05));
	Wire.write(_dec_to_bcd(month));
	Wire.endTransmission();
}

void CDS3231::set_date(byte date)
{
    Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x04));
	Wire.write(_dec_to_bcd(date));
	Wire.endTransmission();
}

void CDS3231::set_dow(byte dow)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x03));
	Wire.write(_dec_to_bcd(dow));	
	Wire.endTransmission();
}

void CDS3231::set_hour(byte hour)
{
    Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x02));
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	bool h12 = (Wire.read() & 0b01000000);
	if (h12) {
		if (hour > 12) {
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
	Wire.write(uint8_t(0x02));
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	byte hour = Wire.read();
	if (h12) {
		hour = hour | 0b01000000;
	}
    else {
		hour = hour & 0b10111111;
	}

	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x02));
	Wire.write(hour);
	Wire.endTransmission();
}

void CDS3231::set_minute(byte minute)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x01));
	Wire.write(_dec_to_bcd(minute));
	Wire.endTransmission();
}

void CDS3231::set_second(byte second)
{
	// This function also resets the Oscillator Stop Flag,
    // which is set whenever power is interrupted.
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x00));
	Wire.write(_dec_to_bcd(second));
	Wire.endTransmission();

	// Clear OSF flag
	byte temp = _read_control_byte(1);
	_write_control_byte((temp & 0b01111111), 1);
}

float CDS3231::get_temperature()
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x11));
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 2);
	byte temp_msb = Wire.read();
	return float(temp_msb) + 0.25 * (Wire.read() >> 6);
}

void CDS3231::get_alarm1(byte &day, byte &hour, byte &minute, byte &second, byte &alarm_bits, bool &dy, bool &h12, bool &PM)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x07));
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 4);
    // 07H
	byte temp = Wire.read();
	second = _bcd_to_dec(temp & 0b01111111);
	alarm_bits = (temp & 0b10000000) >> 7;
    // 08H
	temp = Wire.read();
	minute = _bcd_to_dec(temp & 0b01111111);
	alarm_bits = alarm_bits | (temp & 0b10000000) >> 6;
    // 09H
	temp = Wire.read();
	h12 = temp & 0b01000000;
	if (h12) {
		PM	= temp & 0b00100000;
		hour = _bcd_to_dec(temp & 0b00011111);
	}
    else {
		hour = _bcd_to_dec(temp & 0b00111111);
	}
    alarm_bits = alarm_bits | (temp & 0b10000000) >> 5;
    // 0AH
	temp = Wire.read();
	dy = (temp & 0b01000000) >> 6;
	if (dy) {
		day	= _bcd_to_dec(temp & 0b00001111);
	}
    else {
		day	= _bcd_to_dec(temp & 0b00111111);
	}
    alarm_bits = alarm_bits | (temp & 0b10000000) >> 4;
}

void CDS3231::get_alarm2(byte &day, byte &hour, byte &minute, byte &alarm_bits, bool &dy, bool &h12, bool &PM)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x0b));
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 3);
    // 0BH
	byte temp = Wire.read();
	minute = _bcd_to_dec(temp & 0b01111111);
	alarm_bits = (temp & 0b10000000) >> 3;
    // 0CH
	temp = Wire.read();
	h12 = temp & 0b01000000;
	if (h12) {
		PM = temp & 0b00100000;
		hour = _bcd_to_dec(temp & 0b00011111);
	}
    else {
		hour = _bcd_to_dec(temp & 0b00111111);
	}
    alarm_bits = alarm_bits | (temp & 0b10000000) >> 2;
    // 0DH
	temp = Wire.read();
	dy = (temp & 0b01000000) >> 6;
	if (dy) {
		day	= _bcd_to_dec(temp & 0b00001111);
	}
    else {
		day = _bcd_to_dec(temp & 0b00111111);
	}
    alarm_bits = alarm_bits | (temp & 0b10000000) >> 1;
}

void CDS3231::set_alarm1(byte day, byte hour, byte minute, byte second, byte alarm_bits, bool dy, bool h12)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x07));
    // 07H
	Wire.write(_dec_to_bcd(second) | ((alarm_bits & 0b00000001) << 7));
    // 08H
	Wire.write(_dec_to_bcd(minute) | ((alarm_bits & 0b00000010) << 6));
    // 09H
    byte temp;
    bool PM = false;
	if (h12) {
		if (hour > 12) {
			hour = hour - 12;
			PM = true;
		}
		if (PM) {
			temp = _dec_to_bcd(hour) | 0b01100000;
		}
        else {
			temp = _dec_to_bcd(hour) | 0b01000000;
		}
	}
    else {
		temp = _dec_to_bcd(hour);
	}
	temp = temp | ((alarm_bits & 0b00000100) << 5);
	Wire.write(temp);
    // 0AH
	temp = ((alarm_bits & 0b00001000) << 4) | _dec_to_bcd(day);
	if (dy) {
		temp = temp | 0b01000000;
	}
	Wire.write(temp);
	Wire.endTransmission();
}

void CDS3231::set_alarm2(byte day, byte hour, byte minute, byte alarm_bits, bool dy, bool h12)
{
	Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x0b));
	// 0BH
	Wire.write(_dec_to_bcd(minute) | ((alarm_bits & 0b00010000) << 3));
    // 0CH
    byte temp;
    bool PM = false;
	if (h12) {
		if (hour > 12) {
			hour = hour - 12;
			PM = true;
		}
		if (PM) {
			temp = _dec_to_bcd(hour) | 0b01100000;
		}
        else {
			temp = _dec_to_bcd(hour) | 0b01000000;
		}
	}
    else {
		temp = _dec_to_bcd(hour);
	}
	temp = temp | ((alarm_bits & 0b00100000) << 2);
	Wire.write(temp);
    // 0DH
	temp = ((alarm_bits & 0b01000000) << 1) | _dec_to_bcd(day);
	if (dy) {
		temp = temp | 0b01000000;
	}
	Wire.write(temp);
	Wire.endTransmission();
}

void CDS3231::turn_on_alarm(byte alarm)
{
	byte temp = _read_control_byte(0);
    if (1 == alarm) {
	    temp = temp | 0b00000101;
    }
    else {
        temp = temp | 0b00000110;
    }
	_write_control_byte(temp, 0);
}

void CDS3231::turn_off_alarm(byte alarm)
{
	byte temp = _read_control_byte(0);
    if (1 == alarm) {
	    temp = temp & 0b11111110;
    }
    else {
        temp = temp & 0b11111101;
    }
	_write_control_byte(temp, 0);
}

bool CDS3231::is_alarm_enabled(byte alarm)
{
    byte result = 0x00;
	byte temp = _read_control_byte(0);
	if (1 == alarm) {
		result = temp & 0b00000001;
	}
    else {
		result = temp & 0b00000010;
	}
	return result;
}

bool CDS3231::is_alarming(byte alarm)
{
	byte result = 0x00;
	byte temp = _read_control_byte(1);
	if (1 == alarm) {
		result = temp & 0b00000001;
		temp = temp & 0b11111110;
	}
    else {
		result = temp & 0b00000010;
		temp = temp & 0b11111101;
	}
	_write_control_byte(temp, 1);
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
		// second control byte
		Wire.write(uint8_t(0x0f));
	}
    else {
		// first control byte
		Wire.write(uint8_t(0x0e));
	}
	Wire.endTransmission();

	Wire.requestFrom(CLOCK_ADDRESS, 1);
	return Wire.read();	
}

void CDS3231::_write_control_byte(byte control, bool which)
{
    Wire.beginTransmission(CLOCK_ADDRESS);
	if (which) {
		Wire.write(uint8_t(0x0f));
	}
    else {
		Wire.write(uint8_t(0x0e));
	}
	Wire.write(control);
	Wire.endTransmission();
}

