extern volatile unsigned long timer_tick;
volatile unsigned long i = 0;
void delay_ms(unsigned int time_ms)
{
	timer_tick=time_ms;
	while(timer_tick);
	
}
