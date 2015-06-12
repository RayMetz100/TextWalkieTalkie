#include "LEDFunctions.h"
#include "bsp.h"

void LEDAllOff()
{
	BSP_ledAOff();
  BSP_ledBOff();
  BSP_ledCOff();
  BSP_ledDOff();
  BSP_ledEOff();
	
	BSP_led0Off();
  BSP_led1Off();
  BSP_led2Off();
  BSP_led3Off();
  BSP_led4Off();
  BSP_led5Off();
  BSP_led6Off();
  BSP_led7Off();
};
void LEDAllOn()
{
	BSP_ledAOn();
  BSP_ledBOn();
  BSP_ledCOn();
  BSP_ledDOn();
  BSP_ledEOn();
	
	BSP_led0On();
  BSP_led1On();
  BSP_led2On();
  BSP_led3On();
  BSP_led4On();
  BSP_led5On();
  BSP_led6On();
  BSP_led7On();
};

void LEDXYOnSingle(int x, int y)
{
	// x axis is A - E
	switch (x){
		case 0:{
			BSP_ledAOn();
			break;}
		case 1:{
			BSP_ledBOn();
			break;}
		case 2:{
			BSP_ledCOn();
			break;}
		case 3:{
			BSP_ledDOn();
			break;}
		case 4:{
			BSP_ledEOn();
			break;}
		default:
			break;
	}		
	
	// y axis is 0 - 7
	switch (y){
		case 0:{
			BSP_led0On();
			break;}
		case 1:{
			BSP_led1On();
			break;}
		case 2:{
			BSP_led2On();
			break;}
		case 3:{
			BSP_led3On();
			break;}
		case 4:{
			BSP_led4On();
			break;}
		case 5:{
			BSP_led5On();
			break;}
		case 6:{
			BSP_led6On();
			break;}
		case 7:{
			BSP_led7On();
			break;}
		default:
			break;
	}		
};

//end of file

