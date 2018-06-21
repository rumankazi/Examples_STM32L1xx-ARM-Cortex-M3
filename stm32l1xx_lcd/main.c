#include "stm32l1xx.h"                  // Device header
#include "lcd.h"
#include "delay.h"
#define TIMER_TICK_HZ 1000u
//please press reset button twice to test
volatile unsigned long timer_tick;

void SysTick_Handler(void)
{
  if(timer_tick > 0)
	  --timer_tick;
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
	// This configures interrupt such that SysTick_Handler is called
	// at ever TIMER_TICK_HZ i.e. 1/1000 = 1ms
	SysTick_Config(SystemCoreClock / TIMER_TICK_HZ);
	// Initialize the lcd	
	lcd_init();
  lcd_clear();
  lcd_gotoxy(0, 4);	
	lcd_puts(" LCD TEST ");
	lcd_gotoxy(1, 4);
	lcd_puts(" Done!  ");
	while(1)
	{
		// Set PORTC.8
		GPIOC->BSRR = (1 << 8);
		delay_ms(100);
		// Reset PORTC.8
		GPIOC->BSRR = (1 << 24);
		delay_ms(100);
	}
}
