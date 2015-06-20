#include "bsp.h"

void ButtonAllRowOff(void)
{
	// sets button voltage to 0V
	BSP_buttonR0Off();
	BSP_buttonR1Off();
	BSP_buttonR2Off();
	BSP_buttonR3Off();
}

void ButtonAllRowOn(void)
{
	// sets button voltage to +3.3V
	BSP_buttonR0On();
	BSP_buttonR1On();
	BSP_buttonR2On();
	BSP_buttonR3On();
}

int ButtonCheckPress(int x, int y)
{
	int r = 0; // return code holder
	int t = 0; // time waster (to wait for values to change)
	int i = 0;

	
	ButtonAllRowOff(); //+0V
	
	// first add voltage to correct y axis row
		// y axis is 0 - 3
	switch (y){
		case 0:{
			BSP_buttonR0On();
			break;}
		case 1:{
			BSP_buttonR1On();
			break;}
		case 2:{
			BSP_buttonR2On();
			break;}
		case 3:{
			BSP_buttonR3On();
			break;}
		default:
			break;
	}
	
	// wait some cycles for x axis voltage to stabilize

	for( i = 0 ; i < 10000 ; i++ )
	{
	t++;
	t--;
	}


	// x axis is left to right
	// check for voltage
	switch (x){
		case 0:{
			if(BSP_buttonCS0() == 1) r = 1;
			break;}
		case 1:{
			if(BSP_buttonCS1() == 1) r = 1;
			break;}
		case 2:{
			if(BSP_buttonCS2() == 1) r = 1;
			break;}
		case 3:{
			if(BSP_buttonCS3() == 1) r = 1;
			break;}
		default:
			break;
	}

	//turn off the y axis voltage
	ButtonAllRowOff();
	
	return r;
}

// end of file
