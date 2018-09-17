/**
 *  Copyright (c) 2018 La Cool Co SAS
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 */

#ifndef COOLWIFI_H
#define COOLWIFI_H

#include <Arduino.h>
#ifndef ARDUINO_ARCH_ESP32
#include <ESP8266WiFiMulti.h>
#else
#include <WiFiMulti.h>
#endif
#include "CoolBoardLed.h"

class CoolWifi {

public:
#ifndef ARDUINO_ARCH_ESP32
  ESP8266WiFiMulti wifiMulti;
#else
  WiFiMulti wifiMulti;
#endif
  static void printStatus(wl_status_t status);
  bool config();
  void connect();
#ifndef ARDUINO_ARCH_ESP32
  void startAccessPoint(CoolBoardLed &led);
#endif
  bool getPublicIp(String &ip);
  uint8_t wifiCount = 0;
private:
  bool addWifi(String ssid, String pass);
  void printConf(String ssid[]);
  uint8_t timeOut = 180;
};

#endif
