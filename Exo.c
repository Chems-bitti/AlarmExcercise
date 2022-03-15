
#include "LPC17xx.h"

/* Interrupt Handler for when we get a new sample 
 * Checks the value of the temperature
 * Activates the PWM signals accordignly
 */
void EINT3_IRQHandler() {
  LPC_GPIOINT->IO0IntClr |= 1 << 2; // Clear the pending interrupt register
	unsigned int temp = ((LPC_GPIO0->FIOPIN & (0xFF << 16)) >> 16); // Get the temperature;
	if((temp > 35) && (temp < 45 ) && (temp < 50)) {
		LPC_PWM1->MR2 = 25E6/60E3 * 0.5; // Set the duty cycle of PWM1.2 to 50% using Match Register 2
		LPC_PWM1->LER |= 1 << 2; // Load the new value of Match Register 2
		LPC_PWM1->TCR |= 1; // Launch the PWM timer;
	} else if((temp > 45) && (temp < 50)) {
		LPC_PWM1->MR4 =25E6/60E3*0.9;// Set the duty cycle of PWM1.4 and PWM1.2to 90% using Match Register 3 and 2
		LPC_PWM1->MR2 =25E6/60E3*0.9;
		LPC_PWM1->LER |= 5 << 2;// Update Match Register 4 and 2
	} else if(temp > 50) {
		// Launch timers 0 and 1
		LPC_TIM0->TCR |= 1;
		LPC_TIM2->TCR |= 1;
	} else {
    // Set the duty cycle to 
		LPC_PWM1->MR4 = 0;
		LPC_PWM1->MR2 = 0;
		LPC_PWM1->LER |= 3 << 2;// Update MR2 and 4
		LPC_PWM1->TCR &= ~(9 << 2); // Stop PWM mode and timer
    // Stop the timers, might be redundant, see init_timer 2;
		LPC_TIM0->TCR &= ~1; 
		LPC_TIM2->TCR &= ~1;
		LPC_GPIO2->FIOPIN &= ~(1<<6); // set the output of the alarm to 0;
		
	}
}
/* Timer0 Interrupt Handler
 * Generates the PWM signal, switches P2.6 every 1/30KHz seconds
 */
void TIMER0_IRQHandler() {
	LPC_TIM0->IR |= 1; // Clear interrupt flag
	LPC_GPIO2->FIOPIN ^= (1 << 6); // Switch pin state
}
/* Timer2 Interrupt Handler
 * Stops Timer0 after 30 seconds
 */
void TIMER2_IRQHandler() {
	LPC_TIM0->IR |= 1; 
	LPC_TIM0->TCR &= ~(1); // stop timer
	LPC_GPIO2->FIOPIN &= ~(1<<6); // Reset P2.6 to 0
}
/* Initialize Timer 0
 * Set the Match register Values
 * Activate interruptions
 */
void init_TIMER0(){
  LPC_TIM0->MR0 = 25E6/30E3;// Every half periode, an interrupt is generated to switch P2.6
  LPC_TIM0->MCR |= 3; // Interrupt and reset when you reach MR0;
	NVIC_EnableIRQ(TIMER0_IRQn); // Enable the Interrupt 
}
/* Initialize TIMER 2
 * Set the match register values
 * Activate interruptions
 */
void init_TIMER2() {
	LPC_TIM2->MR0 = 25E6*30; // Period is 30 seconds
	LPC_TIM2->MCR |= 7; // Generate an interrupt and stop the timer when you reach MR0
	NVIC_EnableIRQ(TIMER2_IRQn); // Enable Interrupt
}
     
/* Initialize PWM   */
void init_PWM() {
	LPC_PINCON->PINSEL3 |= (1 << 9) | (1 << 15); // Configure P1.20 and P1.23 to PWM1.2 and PWM1.3 
	LPC_PWM1->PCR |= (5 << 10); // Activate PWM1.2 and PWM1.4
	LPC_PWM1->MR0 = 25E6/60E3; // Set frequency to 60KHz
  	LPC_PWM1->LER |= 1; // Update MR0	
	LPC_PWM1->MR2 = 0; // 0% Duty cycle for PWM1.2
	LPC_PWM1->MR4 = 0; // 0% Duty cycle for PWM1.4
	LPC_PWM1->MCR |= 2; // Reset timer when you reach MR0
	LPC_PWM1->TCR |= 1 << 3; // Activate PWM Mode
}
void init_GPIO() {
	LPC_PINCON->PINSEL0 &= ~(3 << 4); // Set P0.2 to GPIO mode
	LPC_GPIO0->FIODIR &= ~(1 << 2); // Set P0.2 as input
	LPC_PINCON->PINSEL1 &= ~( 0xFFFF ); // P0.16->P0.23 as GPIO
	LPC_GPIO0->FIODIR &= ~(0xFF << 16); // P0.16->P0.23 as input
	
	LPC_PINCON->PINSEL4 &= ~(3<<12); // P2.6 as GPIO
	LPC_GPIO2->FIODIR |= 1 << 6; // P2.6 as output
	LPC_GPIO2->FIOPIN &= ~(1<<6); // Reset P2.6 to 0
	
}

void init_IR() {
	LPC_GPIOINT->IO0IntEnF |= 1 << 2; // Generate interrupt if there's a new sample, so if P0.2 becomes 0 (falling edge)
	LPC_GPIOINT->IO0IntClr |= 1 << 2; // Clear pending interrupts
	NVIC_EnableIRQ(EINT3_IRQn); // Enable interrupt
}
int main() {
	// Initialize everything
	init_GPIO();
	init_IR();
	init_PWM();
	init_TIMER0();
	init_TIMER2();
	// Wait
	while(1);
	
	return 0;
}
