/**
*	\file CoolTime.cpp
*	\brief CoolTime Source File
*	\author Mehdi Zemzem
*	\version 1.0
*	\date 27/06/2017
*
*/


#include "FS.h"

#include "Arduino.h"

#include "CoolTime.h"

#include "ArduinoJson.h"

#include "TimeLib.h"



/**
*	CoolTime::begin():
*	This method is provided to init
*	the udp connection 
*	
*/
void CoolTime::begin()
{
	Serial.println("Entering CoolTime.begin()");
	Serial.println();


	Udp.begin(localPort);

	this->update();

	
}

/**
*	CoolTime::update():
*	This method is provided to correct the
*	rtc Time when it drifts,once every week.
*/
void CoolTime::update()
{
	Serial.println("Entering CoolTime.update()");
	Serial.println();

	if( !( this->isTimeSync() ) )
	{
		Serial.println("waiting for sync");
		Serial.println();

		this->timeSync=this->getNtpTime();
		breakTime(this->getNtpTime(), this->tmSet);
		this->rtc.set(makeTime(this->tmSet), CLOCK_ADDRESS); // set the clock

		this->saveTimeSync();
	}
	
}

/**
*	CoolTime::setDateTime(year,month,dat,hour,minutes,seconds):
*	This method is provided to manually set the RTc Time
*
*/
void CoolTime::setDateTime(int year, int month, int day, int hour, int minutes, int seconds)
{ 
	Serial.println("Entering CoolTime.setDateTime");
	Serial.println();

	tmElements_t tm;
	tm.Second=seconds; 
	tm.Minute=minutes; 
	tm.Hour=hour; 
	tm.Day=day;
	tm.Month=month; 
	tm.Year=year;   

	Serial.print("setting time to : ");//"20yy-mm-ddT00:00:00Z

	Serial.print(tm.Year);
	Serial.print("-");
	Serial.print( this->formatDigits( tm.Month ) );
	Serial.print("-");
	Serial.print( this->formatDigits( tm.Day ) );
	Serial.print("T");
	Serial.print( this->formatDigits( tm.Hour ) );
	Serial.print(":");
	Serial.print( this->formatDigits( tm.Minute ) );
	Serial.print( ":" );
	Serial.print( this->formatDigits( tm.Second ) );
	Serial.print("Z");

	Serial.println();

	this->rtc.set(makeTime(tm),CLOCK_ADDRESS);
	
	Serial.print("time set to : ");
	Serial.println(this->getESDate());
	Serial.println();
}

/**
*	CoolTime::getTimeDate():
*	This method is provided to get the RTC Time
*
*	\returns a tmElements_t structre that has
*	the time in it
*/
tmElements_t CoolTime::getTimeDate()
{	
	Serial.println("Entering CoolTime.getTimeDate()");
	Serial.println();

	tmElements_t tm;
	time_t timeDate = this->rtc.get(CLOCK_ADDRESS);
	breakTime(timeDate,tm);
	
	Serial.print("time is : ");
	Serial.print(tm.Year+ 1970 );
	Serial.print("-");
	Serial.print( this->formatDigits( tm.Month ) );
	Serial.print("-");
	Serial.print( this->formatDigits( tm.Day ) );
	Serial.print("T");
	Serial.print( this->formatDigits( tm.Hour ) );
	Serial.print(":");
	Serial.print( this->formatDigits( tm.Minute ) );
	Serial.print( ":" );
	Serial.print( this->formatDigits( tm.Second ) );
	Serial.print("Z");
	
	return(tm);
}

/**
*	CoolTime::getESD():
*	This method is provided to return an
*	Elastic Search compatible date Format
*	
*	\return date String in Elastic Search
*	format
*/
String CoolTime::getESDate()
{
	Serial.println("Entering CoolTime.getESDate()");
	Serial.println();

	tmElements_t tm=this->getTimeDate();

  	//"20yy-mm-ddT00:00:00Z"
	String elasticSearchString =String(tm.Year+1970)+"-"+this->formatDigits(tm.Month)+"-";

	elasticSearchString +=this->formatDigits(tm.Day)+"T"+this->formatDigits(tm.Hour)+":";
	
	elasticSearchString +=this->formatDigits(tm.Minute)+":"+this->formatDigits(tm.Second)+"Z";
	
	Serial.print("elastic Search date : ");
	Serial.println(elasticSearchString);
	Serial.println();

	return (elasticSearchString);
}

/**
*	CoolTime::getLastSyncTime():
*	This method is provided to get the last time
*	we syncronised the time
*
*	\return unsigned long representation of
*	last syncronisation time in seconds 
*/	
unsigned long CoolTime::getLastSyncTime()
{
	Serial.println("Entering CoolTime.getLastSyncTime()");
	Serial.println();
	
	Serial.print("last sync time : ");
	Serial.println(this->timeSync);

	return(this->timeSync);
}


/**
*	CoolTime::isTimeSync( time in seconds):
*	This method is provided to test if the
*	time is syncronised or not.
*	By default we test once per week.
*
*	\return true if time is syncronised,false
*	otherwise
*/
bool CoolTime::isTimeSync(unsigned long seconds)
{
	Serial.println("Entering CoolTime.isTimeSync() ");
	Serial.println();

//default is once per week we try to get a time update
	if( ( RTC.get(CLOCK_ADDRESS) - this->getLastSyncTime() ) > ( seconds ) ) 
	{
		Serial.println("time is not syncronised ");
		return(false);	
	}
	Serial.println("time is syncronised ");
	return(true);
}


/**
*	CoolTime::getNtopTime():
*	This method is provided to get the
*	Time through an NTP request to
*	a Time Server
*
*	\return a time_t (unsigned long ) timestamp in seconds
*/
time_t CoolTime::getNtpTime()
{
	Serial.println("Entering CoolTime.getNtpTime()");
	Serial.println();

	while (Udp.parsePacket() > 0) ; // discard any previously received packets
	
	Serial.println("Transmit NTP Request");

	sendNTPpacket(timeServer);

	uint32_t beginWait = millis();

	while (millis() - beginWait < 1500) 
	{
		int size = Udp.parsePacket();
		if (size >= NTP_PACKET_SIZE) 
		{
			Serial.println("Receive NTP Response");
			Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
			unsigned long secsSince1900;
			// convert four bytes starting at location 40 to a long integer
			secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
			secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
			secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
			secsSince1900 |= (unsigned long)packetBuffer[43];
			
			Serial.print("received unix time : ");
			Serial.println(secsSince1900 - 2208988800UL);
			Serial.println();

			//Serial.print("received unix time +");
			//Serial.print(this->timeZone);
			//Serial.print(" : ");
			//Serial.println( secsSince1900 - 2208988800UL + this->timeZone * SECS_PER_HOUR );
			//Serial.println();
			
			return secsSince1900 - 2208988800UL ;
		}
	}
	Serial.println("No NTP Response :-(");
	return 0; // return 0 if unable to get the time
}

/**
*	CoolTime::sendNTPpacket( Time Server IP address):
*	This method is provided to send an NTP request to 
*	the time server at the given address
*/ 
void CoolTime::sendNTPpacket(IPAddress &address)
{
	Serial.println("Enter CoolTime.sendNTPpacket()");
	Serial.println();

	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12]  = 49;
	packetBuffer[13]  = 0x4E;
	packetBuffer[14]  = 49;
	packetBuffer[15]  = 52;
	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:                 
	Udp.beginPacket(address, 123); //NTP requests are to port 123
	Udp.write(packetBuffer, NTP_PACKET_SIZE);
	Udp.endPacket();	
}

/**
*	CoolTime::config(time Zone, Time server IP , udp Port):
*	This method is provided to do manual configuration.
*	
*/
void CoolTime::config(int timeZone,IPAddress timeServer,unsigned int localPort)
{
	Serial.println("Enter CoomTime.config() , no SPIFFS variant ");
	Serial.println();

	this->timeZone=timeZone;
	this->timeServer=timeServer;
	this->localPort=localPort;
	
} 

/**
*	CoolTime::config():
*	This method is provided to configure
*	the CoolTime object through a configuration
*	file.
*
*	\return true if successful,false otherwise
*/
bool CoolTime::config()
{
	Serial.println("Enter CoolTime.config()");
	Serial.println();

	File rtcConfig = SPIFFS.open("/rtcConfig.json", "r");

	if (!rtcConfig) 
	{
		Serial.println("failed to read /rtcConfig.json");
		Serial.println();

		return(false);
	}
	else
	{
		size_t size = rtcConfig.size();
		// Allocate a buffer to store contents of the file.
		std::unique_ptr<char[]> buf(new char[size]);

		rtcConfig.readBytes(buf.get(), size);
		DynamicJsonBuffer jsonBuffer;
		JsonObject& json = jsonBuffer.parseObject(buf.get());
		if (!json.success()) 
		{
			Serial.println("failed to parse json");
			Serial.println();

			return(false);
		} 
		else
		{  	
			Serial.println("configuration json is :");
			json.printTo(Serial);
			Serial.println();

			String ip;
			
			if(json["timeZone"].success() )
			{
				this->timeZone=json["timeZone"] ;
			}
			else
			{
				this->timeZone=this->timeZone;			
			}
			json["timeZone"]=this->timeZone;
			
			if(json["timeServer"].success() )
			{			
				 ip=json["timeServer"].as<String>();
				this->timeServer.fromString(ip);
 				
			}
			else
			{
				this->timeServer=this->timeServer;
			}
			json["timeServer"]=ip;
			
			if(json["localPort"].success() )
			{						
				this->localPort=json["localPort"];
			}
			else
			{
				this->localPort=this->localPort;
			}
			json["localPort"]=this->localPort;


			if( json["timeSync"].success() )
			{
				this->timeSync=json["timeSync"];
			}
			else
			{
				this->timeSync=this->timeSync;
			}
			json["timeSync"]=this->timeSync;


			rtcConfig.close();
			rtcConfig= SPIFFS.open("/rtcConfig.json", "w");
			
			if(!rtcConfig)
			{
				Serial.println("failed to write to /rtcConfig.json");
				Serial.println();

				return(false);
			}
			
			json.printTo(rtcConfig);
			rtcConfig.close();

			Serial.println("configuration is :");
			json.printTo(Serial);
			Serial.println();
		
			return(true); 
		}
	}	



}


/**
*	CoolTime::saveTimeSync()
*	This method is provided to
*	save last syncronisation time
*	to the SPIFFS
*/
bool CoolTime::saveTimeSync()
{
	Serial.println("Enter CoolTime.saveTimeSync()");
	Serial.println();

	File rtcConfig = SPIFFS.open("/rtcConfig.json", "r");

	if (!rtcConfig) 
	{
		Serial.println("failed to read /rtcConfig.json");
		Serial.println();

		return(false);
	}
	else
	{
		size_t size = rtcConfig.size();
		// Allocate a buffer to store contents of the file.
		std::unique_ptr<char[]> buf(new char[size]);

		rtcConfig.readBytes(buf.get(), size);
		DynamicJsonBuffer jsonBuffer;
		JsonObject& json = jsonBuffer.parseObject(buf.get());
		if (!json.success()) 
		{
			Serial.println("failed to parse json");
			Serial.println();

			return(false);
		} 
		else
		{  	
			Serial.println("configuration json is :");
			json.printTo(Serial);
			Serial.println();

			String ip;
			
			if(json["timeZone"].success() )
			{
				this->timeZone=json["timeZone"] ;
			}
			else
			{
				this->timeZone=this->timeZone;			
			}
			json["timeZone"]=this->timeZone;
			
			if(json["timeServer"].success() )
			{			
				 ip=json["timeServer"].as<String>();
				this->timeServer.fromString(ip);
 				
			}
			else
			{
				this->timeServer=this->timeServer;
			}
			json["timeServer"]=ip;
			
			if(json["localPort"].success() )
			{						
				this->localPort=json["localPort"];
			}
			else
			{
				this->localPort=this->localPort;
			}
			json["localPort"]=this->localPort;


			if( json["timeSync"].success() )
			{
				json["timeSync"]=this->timeSync;
			}
			else
			{
				this->timeSync=this->timeSync;
			}
			json["timeSync"]=this->timeSync;


			rtcConfig.close();
			rtcConfig= SPIFFS.open("/rtcConfig.json", "w");
			
			if(!rtcConfig)
			{
				Serial.println("failed to write timeSync to /rtcConfig.json");
				Serial.println();

				return(false);
			}
			
			json.printTo(rtcConfig);
			rtcConfig.close();

			Serial.println("configuration is :");
			json.printTo(Serial);
			Serial.println();
		
			return(true); 
		}
	}	



}

/**
*	CoolTime::printConf():
*	This method is provided to print
*	the CoolTime configuration to the
*	Serial Monitor
*/
void CoolTime::printConf()
{
	Serial.println("Entering CoolTime.printConf()");
	Serial.println();

	Serial.println("RTC Configuration") ;

	//Serial.print("timeZone : ");
	//Serial.println(timeZone);

	Serial.print("timeServer : ");
	Serial.println(timeServer);
	
	Serial.print("localPort : :");
	Serial.println(localPort);
}

/**
*	CoolTime::printDigits(digit)
*
*	utility function for digital clock display
*	adds leading 0
*	
*	\return formatted string of the input digit
*/
String CoolTime::formatDigits(int digits)
{
	//Serial.println("Entering CoolTime.formatDigits()");
 	//Serial.println();

	if(digits < 10)
	{
		//Serial.println("output digit : ");
		//Serial.println( String("0") + String(digits) );
		return( String("0") + String(digits) );
	}
	
	//Serial.println("output digit : ");
	//Serial.println(digits);
	return( String(digits) );
}
