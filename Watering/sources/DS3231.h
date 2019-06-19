#ifndef _DS3231_H
#define _DS3231_H
#include <Arduino.h>
#include <Wire.h>


#define CLOCK_ADDRESS     0x68


/**
* @date: 2019.6.19
* @brief:
* 时钟芯片DS3231驱动程序，采用IIC协议。
* Alarm Mask:
*	Dy	A1M4	A1M3	A1M2	A1M1	Rate
*	X	1		1		1		1		Once per second
*	X	1		1		1		0		Alarm when seconds match
*	X	1		1		0		0		Alarm when min, sec match
*	X	1		0		0		0		Alarm when hour, min, sec match
*	0	0		0		0		0		Alarm when date, h, m, s match
*	1	0		0		0		0		Alarm when DoW, h, m, s match
*
*	Dy	A2M4	A2M3	A2M2	Rate
*	X	1		1		1		Once per minute (at seconds = 00)
*	X	1		1		0		Alarm when minutes match
*	X	1		0		0		Alarm when hours and minutes match
*	0	0		0		0		Alarm when date, hour, min match
*	1	0		0		0		Alarm when DoW, hour, min match
* 
* 1. IIC器件地址0x68
* 1. Hour: 12小时制和24小时制
* 2. Month：century当年份从99变为00时触发，暂时忽略。
*/
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
  // Not tested...
  void enable_oscillator(bool TF, bool battery, byte frequency);
  void enable_32kHz_output(bool TF);
  bool is_oscillator_running();

protected:
    byte _dec_to_bcd(byte val);
    byte _bcd_to_dec(byte val);
    byte _read_control_byte(bool which);
    void _write_control_byte(bool which, byte val);
};

#endif
