#include "esp8622.h"


CESP8622 g_esp8622;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  g_esp8622.setup();  
}

void loop() {
  // put your main code here, to run repeatedly: 
  String cmd =  Serial.readString();

  // Common AT comands
  if (cmd.equals("TEST")) {
    if (g_esp8622.comm_test()) {
      Serial.println("comm_test success");
    }
    else {
      Serial.println("comm_test error");
    }
  }
  else if (cmd.equals("RST")) {
    if (g_esp8622.comm_reset()) {
      Serial.println("comm_reset success");
    }
    else {
      Serial.println("comm_reset error");
    }
  }
  else if (cmd.equals("VER")) {
    String ver = "";
    if (g_esp8622.comm_get_version(ver)) {
      Serial.println(ver);
      Serial.println("comm_get_version success");
    }
    else {
      Serial.println("comm_get_version error");
    }
  }
  else if (cmd.equals("DEEP_SLEEP")) {
    if (g_esp8622.comm_deep_sleep(0)) {
      Serial.println("comm_deep_sleep success");
    }
    else {
      Serial.println("comm_deep_sleep error");
    }
  }
  else if (cmd.equals("ECHO")) {
    if (g_esp8622.comm_set_echo(false)) {
      Serial.println("comm_set_echo success");
    }
    else {
      Serial.println("comm_set_echo success");
    }
  }
  else if (cmd.equals("RESTORE")) {
    if (g_esp8622.comm_restore()) {
      Serial.println("comm_restore success");
    }
    else {
      Serial.println("comm_restore success");
    }
  }
  else if (cmd.equals("UART")) {
    unsigned long baudrate;
    int databits, stopbits, parity, flowctrl;
    if (g_esp8622.comm_get_uart(baudrate, databits, stopbits, parity, flowctrl, false)) {
      String info = String(baudrate) + "," + String(databits) + "," + String(stopbits) + "," + String(parity) + "," + String(flowctrl);
      Serial.println(info);
      Serial.println("comm_get_uart success");
    }
    else {
      Serial.println("comm_get_uart success");
    }
  }
  else if (cmd.equals("SLEEP_MODE")) {
    int mode;
    if (g_esp8622.comm_get_sleep_mode(mode)) {
      Serial.println("comm_get_sleep_mode success");
    }
    else {
      Serial.println("comm_get_sleep_mode success");
    }
  }
}
