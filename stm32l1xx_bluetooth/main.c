/*
*USART2 is for Bluetooth
*USART1 is for Serial Terminal
*Wont be able to use LCD as WSN and LCD have few port pins common
#dont assign all the pins of PORTC as output
*/
#include "stm32l1xx_conf.h"
#include "stm32l1xx.h"  
#include "stm32l1xx_gpio1.h"
#include "delay.h"
#include "util.h"
#include "usart.h"
#define TIMER_TICK_HZ 1000u

#define WSN_PORT GPIOC
#define WSN_GPS_PIN 2
#define WSN_WIFI_PIN 3
#define WSN_BT_PIN 4
#define WSN_ZBEE_PIN 5

volatile unsigned long timer_tick = 0;

void SysTick_Handler(void)
{
  if(timer_tick > 0)
	  --timer_tick;
}

__inline void wsn_select_gps()
{
  WSN_PORT->BSRR = ((1 << WSN_GPS_PIN) | (1 << WSN_WIFI_PIN) | 
                    (1 << WSN_BT_PIN)  | (1 << WSN_ZBEE_PIN));
  
  WSN_PORT->BRR = (1 << WSN_GPS_PIN);
}

__inline void wsn_select_wifi()
{
  WSN_PORT->BSRR = ((1 << WSN_GPS_PIN) | (1 << WSN_WIFI_PIN) | 
                    (1 << WSN_BT_PIN)  | (1 << WSN_ZBEE_PIN));
  
  WSN_PORT->BRR = (1 << WSN_WIFI_PIN);
}

__inline void wsn_select_bt()
{
  WSN_PORT->BSRR = ((1 << WSN_GPS_PIN) | (1 << WSN_WIFI_PIN) | 
                    (1 << WSN_BT_PIN)  | (1 << WSN_ZBEE_PIN));
  
  WSN_PORT->BSRR = (1 << (WSN_BT_PIN+16));
}

__inline void wsn_select_zbee()
{
  WSN_PORT->BSRR = ((1 << WSN_GPS_PIN) | (1 << WSN_WIFI_PIN) | 
                    (1 << WSN_BT_PIN)  | (1 << WSN_ZBEE_PIN));
  
  WSN_PORT->BRR = (1 << WSN_ZBEE_PIN);
}


int main(void)
{	       
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; 	// enable the clock to GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; 	// enable the clock to GPIOB
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 	// enable the clock to GPIOC

	// Put PORTC.8 in output mode
	GPIOC->MODER |= (1 << 16);

	// Put PORTC.9 in output mode
	GPIOC->MODER |= (1 << 18);

	// Put PORTA.0 in input mode
	GPIOA->MODER &= ~(3 << 0);
  
  // Make PORTC.4,.5,.6,.7 as output for select line
  WSN_PORT->MODER |= ((1 << (WSN_GPS_PIN * 2)) | (1 << (WSN_WIFI_PIN * 2)) | 
                      (1 << (WSN_BT_PIN * 2))  | (1 << (WSN_ZBEE_PIN * 2)));
  
  // Pullup mux pins
  WSN_PORT->PUPDR |= ((1 << (WSN_GPS_PIN * 2)) | (1 << (WSN_WIFI_PIN * 2)) | 
                      (1 << (WSN_BT_PIN * 2))  | (1 << (WSN_ZBEE_PIN * 2)));
                      
	// This configures interrupt such that SysTick_Handler is called
	// at ever TIMER_TICK_HZ i.e. 1/1000 = 1ms
	SysTick_Config(SystemCoreClock / TIMER_TICK_HZ);
	
  usart1_init();
  usart2_init();
  // Select BT for usart2 on wsn channel mux
  wsn_select_bt();
  
	usart1_puts("WSN KIT\r\n");	
  usart2_puts("WSN KIT\r\n");	
			delay_ms(100);
	while(1)
	{ 
			usart1_puts("Bluetooth TEST\r\n");
			usart2_puts("Bluetooth TEST\r\n");
			delay_ms(100);
	
	}
  
}
