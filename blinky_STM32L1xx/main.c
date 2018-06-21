/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32l1xx.h"                  // Device header
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "RTE_Components.h"             // Component selection


/*
 * main: initialize and start the system
 */
int main (void) {
	volatile unsigned i=0;	
	RCC ->AHBENR |=(1<<2);//RCC clock ENABLE for port c
	
	GPIOC ->MODER |=(0x00010000);// PORTC8 as output port


		
while(i<100000)
{
	for(i=0;i<20000;i++);
	GPIOC ->ODR |=(0x00000100);  //LED ON

	for(i=0;i<20000;i++);
	GPIOC ->ODR &=~(0x00000100);	//LED OFF

	i++;
}

}
