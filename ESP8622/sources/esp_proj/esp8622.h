#ifndef _ESP8622_H
#define _ESP8622_H
#include <stdint.h>
#include <Arduino.h>
/*******************************************************
# Version: 1.0.0
* AT command based. (2019.1.12_V1.6.2)
* 
* Serial:
* 	Serial0: used for logger
* 	Serial1: used for communication
* 
* Response:
* 	No echo. 'ATE0' must be executed first.
********************************************************/


#define ESP_MAX_WIFI_COUNT	64

#define ESP_LOG(str)	Serial.println(String("[ESP_LOG]: ") + String(str));


typedef struct _esp_wifi_info
{
	int ecn;
	String ssid;
	int rssi;
	String mac;
	int channel;
	int freq_offset;
	int freq_cali;
	int pairwise;
	int group_cipher;
	int bgn;
	int wps;
} esp_wifi_info_t;

typedef struct _esp_station_info
{
	String ip_addr;
	String mac;
} esp_station_info_t;

typedef struct _esp_tcpip_info
{
	int link_id;
	String type;
	String remote_ip;
	int remote_port;
	int local_port;
	int tetype;
} esp_tcpip_info_t;


class CESP8622
{
public:
	CESP8622();
	virtual ~CESP8622();

	void setup();

	// Common AT
	bool comm_test();
	bool comm_reset();
	bool comm_get_version(String &ver);
	bool comm_deep_sleep(int time_ms);
	bool comm_set_echo(int enable);
	bool comm_restore();
	bool comm_get_uart(long &baudrate, int &databits, int &stopbits, int &parity, int &flowctrl, bool flash);
	bool comm_set_uart(long baudrate, int databits, int stopbits, int parity, int flowctrl, bool flash);
	
	bool comm_get_sleep_mode(int &mode);
	bool comm_set_sleep_mode(int mode);
	bool comm_set_wakeup_gpio(int enable, int trigger_GPIO, int trigger_level, int awake_GPIO, int awake_level);

	bool comm_set_rfpower(int power);
	bool comm_get_rfvdd33(int &value);
	bool comm_set_rfvdd33(int value);
	bool comm_set_rfvdd33_auto();

	bool comm_query_sysram(int &value);
	bool comm_quary_adc(int &value);
	
	bool comm_get_ioconfig(int pin, int &mode, int &pull_up);
	bool comm_set_ioconfig(int pin, int mode, int pull_up);
	bool comm_set_iodir(int pin, int dir);
	bool comm_get_iostatus(int pin, int &dir, int &level);
	bool comm_set_iolevel(int pin, int level);

	bool comm_set_sysmsg(int value, bool flash);

	// Wifi AT
	bool wifi_get_mode(int &mode, bool flash);
	bool wifi_set_mode(int mode, bool flash);
	bool wifi_get_dhcp(int &value, bool flash);
	bool wifi_set_dhcp(int mode, int en, bool flash);

	bool wifi_station_status(String &ssid, String &bssid, int &channel, int &rssi, bool flash);
	bool wifi_station_connect(String ssid, String pwd, bool flash);
	bool wifi_station_set_scan(int sort_enable, int mask);
	bool wifi_station_scan(esp_wifi_info_t *wifi_list, int &count);
	bool wifi_station_scan(String ssid, String mac, esp_wifi_info_t &wifi_info);
	bool wifi_station_disconnect();
	bool wifi_station_set_auto_connect(int enable);
	bool wifi_station_get_mac(String &mac, bool flash);
	bool wifi_station_set_mac(String mac, bool flash);
	bool wifi_station_get_ip(String &ip, String &gateway, String &netmask, bool flash);
	bool wifi_station_set_ip(String ip, String gateway, String netmask, bool flash);
	bool wifi_station_get_hostname(String &name);
	bool wifi_station_set_hostname(String name);

	bool wifi_softap_get_config(String &ssid, String &pwd, int &channel, int &enc, int &maxconn,
		int &ssid_hidden, bool flash);
	bool wifi_softap_set_config(String ssid, String pwd, int channel, int enc, int maxconn,
		int ssid_hidden, bool flash);
	bool wifi_softap_get_station(esp_station_info_t *stations, int &count);
	bool wifi_softap_get_dhcp_ranges(int &lease_time, String &start_ip, String &end_ip, bool flash);
	bool wifi_softap_set_dhcp_ranges(int enable, int lease_time, String start_ip, String end_ip, bool flash);
	bool wifi_softap_get_mac(String &mac, bool flash);
	bool wifi_softap_set_mac(String mac, bool flash);
	bool wifi_softap_get_ip(String &ip, String &gateway, String &netmask, bool flash);
	bool wifi_softap_set_ip(String ip, String gateway, String netmask, bool flash);

	bool wifi_start_smart(int type);
	bool wifi_stop_smart();
	bool wifi_set_wps(int enable);
	bool wifi_set_mdns(int enable, String hostname, String server_name, int server_port);
	bool wifi_get_country(int &policy, int &code, int &start_channel, int &channel_count, bool flash);
	bool wifi_set_country(int policy, int code, int start_channel, int channel_count, bool flash);

	// TCP/IP AT
	bool tcpip_get_status(int &stat, esp_tcpip_info_t *status, int &count);
	bool tcpip_domain(String domain, String &ip);
	bool tcpip_tcp_connect(int link_id, String type, String remote_ip, int remote_port, int alive);
	bool tcpip_udp_connect(int link_id, String type, String remote_ip, int remote_port, int local_port, int mode);
	bool tcpip_tcp_send(int link_id, String data);
	bool tcpip_udp_send(int link_id, String data, String remote_ip, int remote_port);
	bool tcpip_send_passthrough(String data);
	bool tcpip_tcp_send(int link_id, String data);
	bool tcpip_udp_send(int link_id, String data, String remote_ip, int remote_port);
	bool tcpip_sendbuf(int link_id, String data, int &seg_id);
	bool tcpip_resetbuf(int link_id);
	bool tcpip_bufstatus(int link_id);
	bool tcpip_checkseg(int link_id, int seg_id);
	bool tcpip_set_closemode(int link_id, int abord);
	bool tcpip_disconnect(int link_id);
	bool tcpip_query_ips(String &station_ip, String &station_mac, String &ap_ip, String &ap_mac);
	bool tcpip_get_multi(int &mode);
	bool tcpip_set_multi(int mode);

	bool tcpip_setup_tcpserver(int mode, int port);
	bool tcpip_get_server_maxlinks(int &value);
	bool tcpip_set_server_maxlinks(int value);
	bool tcpip_get_server_timeout(int &timeout_sec);
	bool tcpip_set_server_timeout(int &timeout_sec);

	bool tcpip_get_send_mode(int &value, bool flash);
	bool tcpip_set_send_mode(int value, bool flash);
	bool tcpip_set_recv_info(int enable);
	bool tcpip_get_recv_mode(int &mode);
	bool tcpip_set_recv_mode(int mode);
	bool tcpip_recv_callback(String &data);
	bool tcpip_recv(int len, String &data);
	bool tcpip_get_recvlen(int &len0, int &len1, int &len2, int &len3, int &len4);

	bool tcpip_ping(String ip);
	bool tcpip_get_sntp_config();
	bool tcpip_set_sntp_config();
	bool tcpip_get_sntp_time(String time);

	bool tcpip_get_dns(String &dns1, String &dns2, bool flash);
	bool tcpip_set_dns(int enable, String dns1, String dns2, bool flash);

protected:
	bool _cmd_send(String cmd);
	bool _cmd_recv(String &result);
	bool _cmd_recv(char *result, int &len);
};

#endif
