/*
*
* This class manages the On Board Led
*
*
*
*/
#ifndef CoolOnBoardLed_H
#define CoolOnBoardLed_H

#include"Arduino.h"

#include <NeoPixelBus.h>


class CoolBoardLed

{

public:

	void begin();			 


	void write(int R, int G, int B);       
	void end();				//delete the dynamic led;

	bool config();
	void printConf();


	//Neo Pixel Led methods :
	void colorFade(int R, int G, int B, int T);

	void blink(int R, int G, int B, int T);

	void fadeIn(int R, int G, int B, int T);

	void fadeOut(int R, int G, int B, int T);

	void strobe(int R, int G, int B, int T); 

	void neoPixelLedBegin();

private:


	NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* neoPixelLed = NULL; //template instance must be dynamic

	byte ledActive;

};

#endif