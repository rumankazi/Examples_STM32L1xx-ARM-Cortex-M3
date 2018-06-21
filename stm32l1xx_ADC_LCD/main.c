#include "stm32l1xx.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_adc.h"
#include <stdio.h>
#include "lcd.h"
#include "delay.h"
#include "util.h"

#define TIMER_TICK_HZ 1000u

volatile unsigned long timer_tick = 0;

//******************************************************************************
void SysTick_Handler(void)
{
  if(timer_tick > 0)
	  --timer_tick;
}
 
int main(void)
{
    char buffer[16];
		int peak=0;
		//int ans=0;
		int i=0;
		int arr[100];
  uint16_t adc_value;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; 	// enable the clock to GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; 	// enable the clock to GPIOB
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 	// enable the clock to GPIOC

	// Put PORTC.8 in output mode
	GPIOC->MODER |= (1 << 16);

	// Put PORTC.9 in output mode
	GPIOC->MODER |= (1 << 18);

	// Put PORTA.0 in input mode
	//GPIOA->MODER &= ~(3 << 0);
 GPIOC ->ODR |=(0x00000100);
	
	// This configures interrupt such that SysTick_Handler is called
	// at ever TIMER_TICK_HZ i.e. 1/1000 = 1ms
	SysTick_Config(SystemCoreClock / TIMER_TICK_HZ);
	
	// Initialize the lcd	
	lcd_init();
  	lcd_puts("  WSN KIT  ");	
	lcd_gotoxy(1, 0);
	lcd_puts("   Sensor Test   ");  
  delay_ms(200);
  
  lcd_clear();

  GPIO_InitTypeDef GPIO_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  ADC_InitTypeDef ADC_InitStructure;
 
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32l1xx_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32l1xx.c file
     */
 
 //Clock enabled for GPIO A and B
 // Port B can be used to set as output.
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //output
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
 
 //setting Port B pin 15 high
  GPIO_SetBits(GPIOB,GPIO_Pin_15); // PB15 High
 
 // Port A pin 1 is used as Analog input (ADC input)
 // Configuration as follows
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;  //Analog Mode
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);   //for port A
 
 //ADC uses High Speed Oscillator (HSI) as clock source
 // refer Datasheet for more information
  /* Enable The HSI (16Mhz) */
  RCC_HSICmd(ENABLE);
 
 
  /* Check that HSI oscillator is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
 
 
  /* ADC1 Configuration ------------------------------------------------------*/
 
  /* Enable ADC1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
 
  /* Deinitialize ADC */
  ADC_DeInit(ADC1);
 
  ADC_CommonStructInit(&ADC_CommonInitStructure);
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC Configuration */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE; // Single channel
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // Continuously back-to-back
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;  //Conversion does not start for rising edge or falling edge
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;  //single conversion
  ADC_Init(ADC1,&ADC_InitStructure);
 
  /*LCD initialization */
 		lcd_init();
		  /* Enable ADC1 */
		  ADC_Cmd(ADC1, ENABLE);
  /* Infinite loop */
  while(1)
  {
		  /* ADC1 regular channel 1 for PA1 configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_192Cycles);
 
  /* Enable ADC1 Power Down during Delay */
  ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, ENABLE);
		
		  /* Wait until ADC1 ON status */
		//ADC_FLAG_ONS is set high when the ADC is ready for conversion
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);
		
		
		
		  /* Start ADC1 Software Conversion */
     ADC_SoftwareStartConv(ADC1);
		
    /* Wait until ADC Channel end of conversion */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

		lcd_clear();
		lcd_gotoxy(0, 0);
      	lcd_puts("  Temperature  ");	
	  lcd_gotoxy(1, 0);
		
		//storing the values of conversion for finding out the peak value
				while(i<500)
		{	adc_value = (ADC_GetConversionValue(ADC1));
		arr[i++]=adc_value;
			delay_ms(1);
		}
		
		//finding peak value
		for(i=1;i<500;i++)
		{if(arr[i]>peak)
			peak=arr[i];
		}
		
		
		// LCD requires String to be printed
		 int_to_str(peak, 5, buffer, sizeof(buffer));
     lcd_gotoxy(1, 1);
     lcd_puts(buffer);
     delay_ms(100);   
		 lcd_clear();
		 lcd_gotoxy(0, 0);
     lcd_puts("  Next Cycle  ");
		 
		 //making ADC ready for next cycle
		 ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
		 peak=0;
		 i=0;
  }
}