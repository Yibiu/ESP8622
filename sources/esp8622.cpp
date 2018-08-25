#include "esp8622.h"


CESP8622::CESP8622()
{
}
CESP8622::~CESP8622()
{
}

void CESP8622::setup()
{
	Serial1.begin(115200);
	Serial1.setTimeout(1000); // 1000ms
}

/***************************************************
* Command:
* 	AT\r\n
* Response:
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_test()
{
	String cmd = "AT\r\n";
	if (!_cmd_send(cmd))
		return false;
		
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n"))
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+RST\r\n
* Response:
* 	\r\nOK\r\n + bytes + \r\nready\r\n
****************************************************/
bool CESP8622::comm_reset()
{
	String cmd = "AT+RST\r\n";
	if (!_cmd_send(cmd))
		return false;

	delay(4000);
	String result;
	if (!_cmd_recv(result))
		return false;

	if (result.startsWith("\r\nOK\r\n") && result.endsWith("\r\nready\r\n"))
		return true;
	
	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+GMR\r\n
* Response:
* 	<AT version info>
* 	<SDK version info>
* 	<compile time>
* 	<Bin version>
* 	\r\nOK\r\n
*
* Example:
* AT version:1.6.2.0(Apr 13 2018 11:10:59)
* SDK version:2.2.1(6ab97e9)
* compile time:Jun  7 2018 19:34:29
* Bin version(Wroom 02):1.6.2
* OK
****************************************************/
bool CESP8622::comm_get_version(String &ver)
{
	String cmd = "AT+GMR\r\n";
	if (!_cmd_send(cmd))
		return false;
		
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n")) {
		ver = result.substring(0, result.length() - strlen("\r\nOK\r\n"));
		return true;
	}
	
	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+GSLP\r\n
* Response:
* 	...
****************************************************/
bool CESP8622::comm_deep_sleep(int time_ms)
{
	return false;
}

/***************************************************
* Command:
* 	ATE0\r\n or ATE1\r\n
* Response:
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_set_echo(int enable)
{
	String cmd = "";
	if (enbale) {
		cmd = "ATE1\r\n";
	}
	else {
		cmd = "ATE0\r\n";
	}
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n"))
		return true;
	
	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+RESTORE\r\n
* Response:
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_restore()
{
	String cmd = "AT+RESTORE\r\n";
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n"))
		return true;
	
	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+UART_CUR?\r\n or AT+UART_DEF?\r\n
* Response:
* 	+UART_CUR/DEF:<baudrate>,<databits>,<stopbits>,<parity>,<flow control>
* 	\r\nOK\r\n
*
* Example:
* +UART_CUR:115273,8,1,0,1
* OK
* +UART_DEF:0,0,0,0,0
* OK
****************************************************/
bool CESP8622::comm_get_uart(int &baudrate, int &databits, int &stopbits, int &parity, 
	int &flowctrl, bool flash)
{
	String cmd = "";
	if (flash) {
		cmd = "AT+UART_DEF?\r\n";
	}
	else {
		cmd = "AT+UART_CUR?\r\n";
	}
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if ((result.startsWith("+UART_CUR:") || result.startsWith("+UART_DEF:")) 
		&& result.endsWith("\r\nOK\r\n")) {
		String param = result.substring(strlen("+UART_CUR:"), result.length() - strlen("\r\nOK\r\n"));

		bool success = false;
		int index = -1;
		do {
			index = param.indexOf(',');
			if (-1 == index)
				break;
			baudrate = param.substring(0, index).toInt();
			param = param.substring(index + 1);
			index = param.indexOf(',');
			if (-1 == index)
				break;
			databits = param.substring(0, index).toInt();
			param = param.substring(index + 1);
			index = param.indexOf(',');
			if (-1 == index)
				break;
			stopbits = param.substring(0, index).toInt();
			param = param.substring(index + 1);
			index = param.indexOf(',');
			if (-1 == index)
				break;
			parity = param.substring(0, index).toInt();
			param = param.substring(index + 1);
			flowctrl = param.toInt();

			success = true;
		} while(false);
		
		if (success)
			return true;
	}
	
	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+UART_CUR=<baudrate>,<databits>,<stopbits>,<parity>,<flow	control> or
* 	AT+UART_DEF=<baudrate>,<databits>,<stopbits>,<parity>,<flow	control>
* Response:
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_set_uart(int baudrate, int databits, int stopbits, int parity, int flowctrl, bool flash)
{
	String cmd = "";
	if (flash) {
		cmd = String("AT+UART_DEF=") + String(baudrate) + String(",") + String(databits) + String(",")
			+ String(stopbits) + String(",") + String(parity) + String(",") + String(flowctrl)
			+ String("\r\n");
	}
	else {
		cmd = String("AT+UART_CUR=") + String(baudrate) + String(",") + String(databits) + String(",")
			+ String(stopbits) + String(",") + String(parity) + String(",") + String(flowctrl)
			+ String("\r\n");
	}
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n"))
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+SLEEP?\r\n
* Response:
* 	+SLEEP:<mode>
* 	\r\nOK\r\n
*
* Example:
* 	+SLEEP:2
* 	OK
****************************************************/
bool CESP8622::comm_get_sleep_mode(int &mode)
{
	String cmd = "AT+SLEEP?\r\n";
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.startsWith("+SLEEP:") && result.endsWith("\r\nOK\r\n")) {
		String param = result.substring(strlen("+SLEEP:"), result.length() - strlen("\r\nOK\r\n"));
		mode = param.toInt();
		return true;
	}

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+SLEEP=<mode>\r\n
* Response:
* 	\r\nERROR\r\n
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_set_sleep_mode(int mode)
{
	String cmd = String("AT+SLEEP=") + String(mode) + String("\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n"))
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+WAKEUPGPIO=<enable>,<trigger_GPIO>,<trigger_level>[,<awake_GPIO>,<awake_level>]\r\n
* Response:
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_set_wakeup_gpio(int enable, int trigger_GPIO, int trigger_level, int awake_GPIO, int awake_level)
{
	String cmd = String("AT+WAKEUPGPIO=") + String(enable) + String(",") + String(trigger_GPIO) 
		+ String(",") + String(trigger_level) + String(",") + String(awake_GPIO) 
		+ String(",") + String(awake_level) + String("\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n"))
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	 AT+RFPOWER=<TX	Power>\r\n
* Response:
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_set_rfpower(int power)
{
	String cmd = String("AT+RFPOWER=") + String(power) + String("\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n"))
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	 AT+RFVDD?\r\n
* Response:
* 	+RFVDD:<VDD33>
* 	\r\nOK\r\n
*
* EXample:
* +RFVDD:65535
* OK
****************************************************/
bool CESP8622::comm_get_rfvdd33(int &value)
{
	String cmd = String("AT+RFVDD?\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.startsWith("+RFVDD:") && result.endsWith("\r\nOK\r\n")) {
		String param = result.substring(strlen("+RFVDD:"), result.length() - strlen("\r\nOK\r\n"));
		value = param.toInt();
		return true;
	}

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	 AT+RFVDD=<VDD33>\r\n
* Response:
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_set_rfvdd33(int value)
{
	String cmd = String("AT+RFVDD=") + String(value) + String("\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n"))
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	 AT+RFVDD\r\n
* Response:
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_set_rfvdd33_auto()
{
	String cmd = String("AT+RFVDD\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n"))
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	 AT+RFAUTOTRACE?\r\n
* Response:
* 	+RFAUTOTRACE:<enable>
* 	\r\nOK\r\n
*
* Example:
* +RFAUTOTRACE:1
* OK
****************************************************/
bool CESP8622::comm_get_rfautotrace(int &value)
{
	String cmd = String("AT+RFAUTOTRACE?\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.startsWith("+RFAUTOTRACE:") && result.endsWith("\r\nOK\r\n")) {
		String param = result.substring(strlen("+RFAUTOTRACE:"), result.length() - strlen("\r\nOK\r\n"));
		value = param.toInt();
		return true;
	}

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	 AT+RFAUTOTRACE=<enable>\r\n
* Response:
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_set_rfautotrace(int value)
{
	String cmd = String("AT+RFAUTOTRACE=") + String(value) + String("\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n"))
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	 AT+SYSRAM?\r\n
* Response:
* 	+SYSRAM:<RAM>
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_query_sysram(int &value)
{
	String cmd = String("AT+SYSRAM?\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.startsWith("+SYSRAM:") && result.endsWith("\r\nOK\r\n")) {
		String param = result.substring(strlen("+SYSRAM:"), result.length() - strlen("\r\nOK\r\n"));
		value = param.toInt();
		return true;
	}

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	 AT+SYSADC?\r\n
* Response:
* 	+SYSADC:<ADC>
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_quary_adc(int &value)
{
	String cmd = String("AT+SYSADC?\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.startsWith("+SYSADC:") && result.endsWith("\r\nOK\r\n")) {
		String param = result.substring(strlen("+SYSADC:"), result.length() - strlen("\r\nOK\r\n"));
		value = param.toInt();
		return true;
	}

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	 AT+SYSIOGETCFG=<pin>\r\n
* Response:
* 	+SYSIOGETCFG:<pin>,<mode>,<pull-up> 
* 	\r\nOK\r\n
*
* Example:
* +SYSIOGETCFG:1,0,0
* OK
****************************************************/
bool CESP8622::comm_get_ioconfig(int pin, int &mode, int &pull_up)
{
	String cmd = String("AT+SYSIOGETCFG=") + String(pin) + String("\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.startsWith("+SYSIOGETCFG:") && result.endsWith("\r\nOK\r\n")) {
		String param = result.substring(strlen("+SYSIOGETCFG:"), result.length() - strlen("\r\nOK\r\n"));
		
		bool success = false;
		int index = -1;
		do {
			index = param.indexOf(',');
			if (-1 == index)
				break;
			pin = param.substring(0, index).toInt();
			param = param.substring(index + 1);
			index = param.indexOf(',');
			if (-1 == index)
				break;
			mode = param.substring(0, index).toInt();
			param = param.substring(index + 1);
			pull_up = param.toInt();

			success = true;
		} while(false);

		if (success)
			return true;
	}

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	 AT+SYSIOGETCFG=<pin>,<mode>,<pull_up>\r\n
* Response:
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_set_ioconfig(int pin, int mode, int pull_up)
{
	String cmd = String("AT+SYSIOGETCFG=") + String(pin) + String(",") + String(mode)
		+ String(",") + String(pull_up) + String("\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n"))
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	 AT+SYSGPIOREAD=<pin>\r\n
* Response:
* 	+SYSGPIOREAD:<pin>,<dir>,<level>
* 	\r\nOK\r\n
*
* 	or
*
* 	NOT	GPIO MODE！
* 	\r\nERROR\r\n
****************************************************/
bool CESP8622::comm_get_iostatus(int pin, int &dir, int &level)
{
	String cmd = String("AT+SYSGPIOREAD=") + String(pin) + String("\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.startsWith("+SYSGPIOREAD:") && result.endsWith("\r\nOK\r\n")) {
		String param = result.substring(strlen("+SYSGPIOREAD:"), result.length() - strlen("\r\nOK\r\n"));

		bool success = false;
		int index = -1;
		do {
			index = param.indexOf(',');
			if (-1 == index)
				break;
			pin = param.substring(0, index).toInt();
			param = param.substring(index + 1);
			index = param.indexOf(',');
			if (-1 == index)
				break;
			dir = param.substring(0, index).toInt();
			param = param.substring(index + 1);
			level = param.toInt();

			success = true;
		} while(false);

		if (success)
			return true;
	}

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	 AT+SYSGPIODIR=<pin>,<dir>\r\n
* Response:
* 	\r\nOK\r\n
*
* 	or
*
* 	NOT	GPIO MODE！
* 	\r\nERROR\r\n
****************************************************/
bool CESP8622::comm_set_iodir(int pin, int dir)
{
	String cmd = String("AT+SYSGPIODIR=") + String(pin) + String(",") + String(dir) + String("\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n")) 
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	 AT+SYSGPIOWRITE=<pin>,<level>\r\n
* Response:
* 	\r\nOK\r\n
*
* 	or
*
* 	NOT	OUTPUT!
* 	\r\nERROR\r\n
****************************************************/
bool CESP8622::comm_set_iolevel(int pin, int level)
{
	String cmd = String("AT+SYSGPIOWRITE=") + String(pin) + String(",") + String(level) + String("\r\n");
	if (!_cmd_send(cmd))
		return false;
	
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n")) 
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+SYSMSG_CUR=<n>\r\n	or
* 	AT+SYSMSG_DEF=<n>\r\n
* Response:
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::comm_set_sysmsg(int value, bool flash)
{
	String cmd = "";
	if (flash) {
		cmd = String("AT+SYSMSG_DEF=") + String(value) + String("\r\n");
	}
	else {
		cmd = String("AT+SYSMSG_CUR=") + String(value) + String("\r\n");
	}
	if (!_cmd_send(cmd))
		return false;

	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n")) 
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+CWMODE_CUR?\r\n		or
* 	AT_CWMODE_DEF?\r\n
* Response:
* 	+CWMODE_CUR/DEF:<mode>
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::wifi_get_mode(int &mode, bool flash)
{
	String cmd = "";
	if (flash) {
		cmd = "AT_CWMODE_DEF?\r\n";
	}
	else {
		cmd = "AT+CWMODE_CUR?\r\n";
	}
	if (!_cmd_send(cmd))
		return false;

	String result = "";
	if (!_cmd_recv(result))
		return false;

	if ((result.startsWith("+CWMODE_CUR:") || result.startsWith("+CWMODE_DEF:"))
		&& result.endsWith("\r\nOK\r\n")) {
		String param = result.substring(strlen("+CWMODE_CUR:"), result.length() - strlen("\r\nOK\r\n"));
		mode = param.toInt();
		return true;
	}

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+CWMODE_CUR=<mode>\r\n		or
* 	AT_CWMODE_DEF=<mode>\r\n
* Response:
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::wifi_set_mode(int mode, bool flash)
{
	String cmd = "";
	if (flash) {
		cmd = String("AT_CWMODE_DEF=") + String(mode) + String("\r\n");
	}
	else {
		cmd = String("AT_CWMODE_CUR=") + String(mode) + String("\r\n");
	}
	if (!_cmd_send(cmd))
		return false;

	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n")
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_get_dhcp(int &value, bool flash)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_set_dhcp(int mode, int en, bool flash)
{
    return false;
}

/***************************************************
* Command:
* 	AT+CWJAP_CUR?\r\n	or
* 	AT_CWJAP_DEF?\r\n
* Response:
* 	+CWJAP_CUR:<ssid>,<bssid>,<channel>,<rssi>
* 	\r\nOK\r\n
*
* 	or
*
* 	No AP
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::wifi_station_status(String &ssid, String &bssid, int &channel, int &rssi, bool flash)
{
	String cmd = "";
	if (flash) {
		cmd = "AT_CWJAP_DEF?\r\n";
	}
	else {
		cmd = "AT+CWJAP_CUR?\r\n";
	}
	if (!_cmd_send(cmd))
		return false;

	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n") {
		if (result.startsWith("+CWJAP_CUR:") || result.startsWith("+CWJAP_DEF:")) {
			String param = result.substring(strlen("+CWJAP_CUR:"), result.length() - strlen("\r\nOK\r\n"));

			bool success = false;
			int index = -1;
			do {
				index = param.indexOf(',');
				if (-1 == index)
					break;
				ssid = param.substring(0, index);
				param = param.substring(index + 1);
				index = param.indexOf(',');
				if (-1 == index)
					break;
				bssid = param.substring(0, index);
				param = param.substring(index + 1);
				index = param.indexOf(',');
				if (-1 == index)
					break;
				channel = param.substring(0, index).toInt();
				param = param.substring(index + 1);
				rssi = param.toInt();

				success = true;
			} while(false);

			if (success)
				return true;
		}
		else {
			ssid = "";
			bssid = "";
			channel = 0;
			rssi = 0;
			return true;
		}
	}

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+CWJAP_CUR=<ssid>,<pwd>\r\n	or
* 	AT_CWJAP_DEF=<ssid>,<pwd>\r\n
* Response:
* 	\r\nOK\r\n
*
* 	or
*
* 	+CWJAP_CUR/DEF:<error code>
* 	\r\nFAIL\r\n
****************************************************/
bool CESP8622::wifi_station_connect(String ssid, String pwd, bool flash)
{
	String cmd = "";
	if (flash) {
		cmd = String("AT+CWJAP_DEF=") + ssid + String(",") + pwd + "\r\n";
	}
	else {
		cmd = String("AT+CWJAP_CUR=") + ssid + String(",") + pwd + "\r\n";
	}
	if (!_cmd_send(cmd))
		return false;

	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n")
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+CWLAPOPT=<sort_enable>,<mask>\r\n
* Response:
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::wifi_station_set_scan(int sort_enable, int mask)
{
	String cmd = String("AT+CWLAPOPT=") + String(sort_enable) + String(",") + String(mask) + String("\r\n");
	if (!_cmd_send(cmd))
		return false;

	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n")
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+CWLAP\r\n
* Response:
* 	+CWLAP:<ecn>,<ssid>,<rssi>,<mac>,<channel>,<freq offset>,<freq cali>,<pairwise_cipher>,
* 	<group_cipher>,<bgn>,<wps>
* 	\r\nOK\r\n
*
* Example:
* +CWLAP:(4,"magewell_006",-57,"14:91:82:2b:1e:8c",1,117,0,4,3,7,1)
* +CWLAP:(0,"magewell_006-璁垮",-57,"16:91:82:2b:1e:8e",1,118,0,0,0,7,0)
* +CWLAP:(3,"magewell",-60,"f0:9f:c2:6c:ce:cf",6,133,0,4,4,7,0)
* +CWLAP:(4,"magewell_005",-66,"14:91:82:2b:1f:6c",6,112,0,4,3,7,1)
* +CWLAP:(4,"magewell_004",-69,"64:09:80:72:39:e9",11,142,0,5,3,7,1)
* +CWLAP:(3,"Ruckus-Radin",-70,"84:18:3a:04:c4:f8",11,32767,0,4,4,4,0)
* +CWLAP:(3,"MWL1",-63,"e4:95:6e:40:ca:72",11,140,0,4,4,7,0)
****************************************************/
bool CESP8622::wifi_station_scan(esp_wifi_info_t *wifi_list, int &count)
{
	String cmd = "AT+CWLAP\r\n";
	if (!_cmd_send(cmd))
		return false;

	delay(5000);
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n") {
		String param = result.substring(0, result.length() - strlen("\r\nOK\r\n"));

		bool success = false;
		count = 0;
		while(param.startsWith("+CWLAP:"))
		{
			int next_index = param.indexOf("+CWLAP:", 1);
			String item = "";
			if (-1 == next_index) {
				item = param;
			}
			else {
				item = param.substring(0, next_index);
			}

			// Parse item
			item = item.substring(strlen("+CWLAP:"));
			item.replace("(", " ");
			item.replace(")", " ");
			item.trim();

			success = false;
			int index = -1;
			do {
				index = item.indexOf(',');
				if (-1 == index)
					break;
				wifi_list[count].ecn = item.substring(0, index).toInt();
				item = item.substring(index + 1);
				index = item.indexOf(',');
				if (-1 == index)
					break;
				wifi_list[count].ssid = item.substring(0, index);
				item = item.substring(index + 1);
				index = item.indexOf(',');
				if (-1 == index)
					break;
				wifi_list[count].rssi = item.substring(0, index).toInt();
				item = item.substring(index + 1);
				index = item.indexOf(',');
				if (-1 == index)
					break;
				wifi_list[count].mac = item.substring(0, index);
				item = item.substring(index + 1);
				index = item.indexOf(',');
				if (-1 == index)
					break;
				wifi_list[count].channel = item.substring(0, index).toInt();
				item = item.substring(index + 1);
				index = item.indexOf(',');
				if (-1 == index)
					break;
				wifi_list[count].freq_offset = item.substring(0, index).toInt();
				item = item.substring(index + 1);
				index = item.indexOf(',');
				if (-1 == index)
					break;
				wifi_list[count].freq_cali = item.substring(0, index).toInt();
				item = item.substring(index + 1);
				index = item.indexOf(',');
				if (-1 == index)
					break;
				wifi_list[count].pairwise = item.substring(0, index).toInt();
				item = item.substring(index + 1);
				index = item.indexOf(',');
				if (-1 == index)
					break;
				wifi_list[count].group_cipher = item.substring(0, index).toInt();
				item = item.substring(index + 1);
				index = item.indexOf(',');
				if (-1 == index)
					break;
				wifi_list[count].bgn = item.substring(0, index).toInt();
				item = item.substring(index + 1);
				wifi_list[count].wps = item.toInt();

				count++;
				success = true;
			} while(false);
			if (!success)
				break;

			if (-1 == next_index)
				break;
			param = param.substring(next_index);
		}

		if (success)
			return true;
	}

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+CWLAP=<ssid>,<mac>\r\n
* Response:
* 	+CWLAP:<ecn>,<ssid>,<rssi>,<mac>,<channel>,<freq offset>,<freq cali>,<pairwise_cipher>,
* 	<group_cipher>,<bgn>,<wps>
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::wifi_station_scan(String ssid, String mac, esp_wifi_info_t &wifi_info)
{
	String cmd = String("AT+CWLAP=") + ssid + String(",") + mac + String("\r\n");
	if (!_cmd_send(cmd))
		return false;

	delay(5000);
	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n") {
		if (result.startsWith("+CWLAP:")) {
			String param = result.substring(strlen("+CWLAP:"), result.length() - strlen("\r\nOK\r\n"));
			param.replace("(", " ");
			param.replace(")", " ");
			param.trim();

			bool success = false;
			int index = -1;
			do {
				index = param.indexOf(',');
				if (-1 == index)
					break;
				wifi_info.ecn = param.substring(0, index).toInt();
				param = param.substring(index + 1);
				index = param.indexOf(',');
				if (-1 == index)
					break;
				wifi_info.ssid = param.substring(0, index);
				param = param.substring(index + 1);
				index = param.indexOf(',');
				if (-1 == index)
					break;
				wifi_info.rssi = param.substring(0, index).toInt();
				param = param.substring(index + 1);
				index = param.indexOf(',');
				if (-1 == index)
					break;
				wifi_info.mac = param.substring(0, index);
				param = param.substring(index + 1);
				index = param.indexOf(',');
				if (-1 == index)
					break;
				wifi_info.channel = param.substring(0, index).toInt();
				param = param.substring(index + 1);
				index = param.indexOf(',');
				if (-1 == index)
					break;
				wifi_info.freq_offset = param.substring(0, index).toInt();
				param = param.substring(index + 1);
				index = param.indexOf(',');
				if (-1 == index)
					break;
				wifi_info.freq_cali = param.substring(0, index).toInt();
				param = param.substring(index + 1);
				index = param.indexOf(',');
				if (-1 == index)
					break;
				wifi_info.pairwise = param.substring(0, index).toInt();
				param = param.substring(index + 1);
				index = param.indexOf(',');
				if (-1 == index)
					break;
				wifi_info.group_cipher = param.substring(0, index).toInt();
				param = param.substring(index + 1);
				index = param.indexOf(',');
				if (-1 == index)
					break;
				wifi_info.bgn = param.substring(0, index).toInt();
				param = param.substring(index + 1);
				wifi_info.wps = param.toInt();

				success = true;
			} while(false);

			if (success)
				return true;
		}
		else {
			wifi_info.ssid = "";
			wifi_info.mac = "";
			return true;
		}
	}

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+CWQAP
* Response:
* 	/r/nOK/r/n
****************************************************/
bool CESP8622::wifi_station_disconnect()
{
	String cmd = String("AT+CWQAP\r\n");
	if (!_cmd_send(cmd))
		return false;

	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n")
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_station_set_auto_connect(int enable)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_station_get_mac(String &mac, bool flash)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_station_set_mac(String mac, bool flash)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_station_get_ip(String &ip, String &gateway, String &netmask, bool flash)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_station_set_ip(String ip, String gateway, String netmask, bool flash)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_station_get_hostname(String &name)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_station_set_hostname(String name)
{
    return false;
}

/***************************************************
* Command:
* 	AT+CWSAP_CUR?\r\n	or
* 	AT+CWSAP_DEF?\r\n
* Response:
* 	+CWSAP_CUR/DEF:<ssid>,<pwd>,<chl>,<ecn>,<maxconn>,<ssid_hidden>
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::wifi_softap_get_config(String &ssid, String &pwd, int &channel, int &enc, int &maxconn,
	int &ssid_hidden, bool flash)
{
	String cmd = "";
	if (flash) {
		cmd = "AT+CWSAP_DEF?\r\n";
	}
	else {
		cmd = "AT+CWSAP_CUR?\r\n";
	}
	if (!_cmd_send(cmd))
		return false;

	String result = "";
	if (!_cmd_recv(result))
		return false;

	if ((result.startsWith("+CWSAP_CUR:") || result.startsWith("+CWSAP_DEF:"))
		&& result.endsWith("\r\nOK\r\n")) {
		String param = result.substring(strlen("+CWSAP_CUR:"), result.length() - strlen("\r\nOK\r\n"));

		bool success = false;
		int index = -1;
		do {
			index = param.indexOf(',');
			if (-1 == index)
				break;
			ssid = param.substring(0, index);
			param = param.substring(index + 1);
			index = param.indexOf(',');
			if (-1 == index)
				break;
			pwd = param.substring(0, index);
			param = param.substring(index + 1);
			index = param.indexOf(',');
			if (-1 == index)
				break;
			channel = param.substring(0, index).toInt();
			param = param.substring(index + 1);
			index = param.indexOf(',');
			if (-1 == index)
				break;
			enc = param.substring(0, index).toInt();
			param = param.substring(index + 1);
			index = param.indexOf(',');
			if (-1 == index)
				break;
			maxconn = param.substring(0, index).toInt();
			param = param.substring(index + 1);
			ssid_hidden = param.toInt();

			success = true;
		} while(false);

		if (success)
			return true;
	}

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+CWSAP_CUR=<ssid>,<pwd>,<chl>,<ecn>,<maxconn>,<ssid_hidden>\r\n	or
* 	AT+CWSAP_DEF=<ssid>,<pwd>,<chl>,<ecn>,<maxconn>,<ssid_hidden>\r\n
* Response:
* 	\r\nOK\r\n
*
* 	or
*
* 	\r\nERROR\r\n
****************************************************/
bool CESP8622::wifi_softap_set_config(String ssid, String pwd, int channel, int enc, int maxconn,
	int ssid_hidden, bool flash)
{
	String cmd = "";
	if (flash) {
		cmd = String("AT+CWSAP_DEF=") + ssid + String(",") + pwd + String(",") + String(channel)
			+ String(",") + String(ecn) + String(",") + String(maxconn)
			+ String(",") + String(ssid_hidden) + String("\r\n");
	}
	else {
		cmd = String("AT+CWSAP_CUR=") + ssid + String(",") + pwd + String(",") + String(channel)
			+ String(",") + String(ecn) + String(",") + String(maxconn)
			+ String(",") + String(ssid_hidden) + String("\r\n");
	}
	if (!_cmd_send(cmd))
		return false;

	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n")
		return true;

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
* 	AT+CWLIF\r\n
* Response:
* 	+CWLIF:<ip>,<mac>
* 	\r\nOK\r\n
****************************************************/
bool CESP8622::wifi_softap_get_station(esp_station_info_t *stations, int &count)
{
	String cmd = "AT+CWLIF\r\n";
	if (!_cmd_send(cmd))
		return false;

	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n") {
		String param = result.substring(0, result.length() - strlen("\r\nOK\r\n"));

        count = 0;
		while (param.startsWith("+CWLIF:"))
		{
			int next_index = param.indexOf("+CWLIF:", 1);
			String item = "";
			if (-1 == next_index) {
				item = param;
			}
			else {
				item = param.substring(0, next_index);
			}
			
            item = item.substring(strlen("+CWLIF:"));
            bool success = false;
            int index = -1;
            do {
                index = item.indexOf(',');
                if (-1 == index)
                    break;
                stations[count].ip_addr = item.substring(0, index);
                item = item.substring(index + 1);
                stations[count].mac = item;

                 count++;
                 success = true;
            } while(false);
            if (!success)
                break;

            if (-1 == next_index)
                break;
            param = param.substring(next_index);
		}

		if (success)
		    return true;
	}

	ESP_LOG(result);
	return false;
}

/***************************************************
* Command:
*   AT+CWDHCPS_CUR?
* Response:
*   +CWDHCPS_CUR=<lease	time>,<start IP>,<end IP>
****************************************************/
bool wifi_softap_get_dhcp_ranges(int &lease_time, String &start_ip, String &end_ip, bool flash)
{
    /*String cmd = "";
	if (flash) {
		cmd = String("AT+CWSAP_DEF=") + ssid + String(",") + pwd + String(",") + String(channel)
			+ String(",") + String(ecn) + String(",") + String(maxconn)
			+ String(",") + String(ssid_hidden) + String("\r\n");
	}
	else {
		cmd = String("AT+CWSAP_CUR=") + ssid + String(",") + pwd + String(",") + String(channel)
			+ String(",") + String(ecn) + String(",") + String(maxconn)
			+ String(",") + String(ssid_hidden) + String("\r\n");
	}
	if (!_cmd_send(cmd))
		return false;

	String result = "";
	if (!_cmd_recv(result))
		return false;

	if (result.endsWith("\r\nOK\r\n")
		return true;

	ESP_LOG(result);
	return false;*/
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_softap_set_dhcp_ranges(int enable, int lease_time, String start_ip, String end_ip, bool flash)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_softap_get_mac(String &mac, bool flash)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_softap_set_mac(String mac, bool flash)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_softap_get_ip(String &ip, String &gateway, String &netmask, bool flash)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_softap_set_ip(String ip, String gateway, String netmask, bool flash)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_start_smart(int type)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_stop_smart()
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_set_wps(int enable)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_set_mdns(int enable, String hostname, String server_name, int server_port)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_get_country(int &policy, int &code, int &start_channel, int &channel_count, bool flash)
{
    return false;
}

/***************************************************
* Command:
*
* Response:
*
****************************************************/
bool wifi_set_country(int policy, int code, int start_channel, int channel_count, bool flash)
{
    return false;
}


bool CESP8622::_cmd_send(String cmd)
{
	int ret = Serial1.write(cmd.c_str());
	return (ret == cmd.length());
}

bool CESP8622::_cmd_recv(String &result)
{
	result = Serial1.readString();
	return true;
}

bool CESP8622::_cmd_recv(char *result, int &len)
{
	len = Serial1.readBytes(result, len);
	return true;
}
