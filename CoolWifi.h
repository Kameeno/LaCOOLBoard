/**
*	\file CoolWifi.h
*	\brief CoolWifi Header File
*	\author Mehdi Zemzem
*	\version 1.0
*	\date 27/06/2017
*
*/

#ifndef CoolWifi_H
#define CoolWifi_H


#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager


/**
*	\class CoolWifi
*  
*	\brief This class manages the WiFi connection .
*  
*/

class CoolWifi
{

public:
	void begin();
	
	bool config();
	
	bool config(String ssid[],String pass[],int wifiNumber,int APTimeOut);

	wl_status_t connect();

	wl_status_t state();
	
	void printConf();
	
	bool addWifi( String ssid , String pass="" );

private:
	
	WiFiManager wifiManager;
	
	ESP8266WiFiMulti wifiMulti;
	
	int wifiCount=0;
	
	String ssid[50]={"0"};
	
	String pass[50]={"0"};
	
	int timeOut=0;//in seconds

		

};

#endif