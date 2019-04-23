#ifndef _DS3231_H
#define _DS3231_H
#include <Arduino.h>
#include <Wire.h>


#define CLOCK_ADDRESS   0x68


class CDS3231
{
public:
    CDS3231();
    virtual ~CDS3231();

    // Time functions
    void get_time(byte &year, byte &month, byte &date, byte &dow,
        bool &h12, bool &PM, byte &hour, byte &minute, byte &second);
    byte get_year();
    byte get_month(bool &century);
    byte get_date();
    byte get_dow();
    byte get_hour(bool &h12, bool &PM);
    byte get_minute();
    byte get_second();

    void set_year(byte year);
    void set_month(byte month);
    void set_date(byte date);
    void set_dow(byte dow);
    void set_hour(byte hour);
    void set_hour_mode(bool h12);
    void set_minute(byte minute);
    void set_second(byte second);

    // Temperature functions
    float get_temperature();

    // Alarm functions
    void get_alarm1(byte &day, byte &hour, byte &minute, byte &second,
        byte &alarm_bits, bool &dy, bool &h12, bool &PM);
    void get_alarm2(byte &day, byte &hour, byte &minute,
        byte &alarm_bits, bool &dy, bool &h12, bool &PM);
    void set_alarm1(byte day, byte hour, byte minute, byte second,
        byte alarm_bits, bool dy, bool h12);
    void set_alarm2(byte day, byte hour, byte minute,
        byte alarm_bits, bool dy, bool h12);
   	void turn_on_alarm(byte alarm);
    void turn_off_alarm(byte alarm);
    bool is_alarm_enabled(byte alarm);
    bool is_alarming(byte alarm);

    // Oscillator functions(Attention to use!!!)
	void enable_oscillator(bool TF, bool battery, byte frequency);
	void enable_32kHz_output(bool TF);
	bool is_oscillator_running();

protected:
    byte _dec_to_bcd(byte val);
    byte _bcd_to_dec(byte val);
    byte _read_control_byte(bool which);
    void _write_control_byte(byte control, bool which);
};

#endif
