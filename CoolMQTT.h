/**
*	\file CoolMQTT.h
*	\brief CoolMQTT Header File
*	\author Mehdi Zemzem
*	\version 1.0
*	\date 27/06/2017
*
*/


#ifndef CoolMQTT_H
#define CoolMQTT_H

#include"Arduino.h"  
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/**
*	\class CoolMQTT
*	\brief This class handles the mqtt client
*/
class CoolMQTT
{

public:

	void begin();
	int connect(uint16_t keepAlive);

	bool publish(const char* data);

	String read();

	void config(const char mqttServer[],const char inTopic[],const char outTopic[],const char clientId[],int bufferSize);
	bool config();

	void callback(char* topic, byte* payload, unsigned int length);

	void printConf();

	int state();

	bool mqttLoop();

private:
	
	char mqttServer[50];
	String msg;
	char inTopic[50];
	char outTopic[50];
	char clientId[50];
	int bufferSize;	
	WiFiClient espClient;
	PubSubClient client;
	bool newMsg;


};

#endif
