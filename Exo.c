
#include "LPC17xx.h"

/* Interrupt Handler for when we get a new sample 
 * Checks the value of the temperature
 * Activates the PWM signals accordignly
 */
void EINT2_IRQHandler() {
  LPC_GPIOINT->IO0IntClr |= 1 << 2; // Clear the pending interrupt register
	unsigned int temp = ((LPC_GPIO0->FIOPIN & (0xFF << 16)) >> 16); // Get the temperature;
	if((temp > 35) && (temp < 45 ) && (temp < 50) {
		LPC_PWM1->MR2 = 25E6/60E3 * 0.5; // Set the duty cycle of PWM1.2 to 50% using Match Register 2
		LPC_PWM1->LER |= 1 << 2; // Load the new value of Match Register 2
		LPC_PWM1->TCR |= 1; // Launch the PWM timer;
	} else if((temp > 45) && (temp < 50)) {
		LPC_PWM1->MR3 =25E6/60E3*0.9;// Set the duty cycle of PWM1.3 and PWM1.2to 90% using Match Register 3 and 2
		LPC_PWM1->MR2 =25E6/60E3*0.9;
		LPC_PWM1->LER |= 3 << 2;// Update Match Register 3 and 2
	} else if(temp > 50) {
		// Launch timers 0 and 1
		LPC_TIM0->TCR |= 1;
		LPC_TIM2->TCR |= 1;
	} else {
    // Set the duty cycle to 
		LPC_PWM1->MR3 =0;
		LPC_PWM1->MR2 = 0;
		LPC_PWM1->LER |= 3 << 2;// Update MR2 and 3
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
	LPC_TIM0->IR |= 1;
	LPC_GPIO2->FIOPIN ^= (1 << 6); // Switch pin state
}
/* Timer2 Interrupt Handler
 * Stops Timer0 after 30 seconds
 */
void TIMER2_IRQHandler() {
	LPC_TIM0->IR |= 1;
	LPC_TIM0->TCR &= ~(1);
	LPC_GPIO2->FIOPIN &= ~(1<<6);
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
	LPC_PINCON->PINSEL3 |= (1 << 9) | (1 << 15); // Mettre P1.20 et P1.23 en PWM1.2 et PWM1.3 repectivement
	LPC_PWM1->PCR |= (3 << 10); // Activation du PWM1.2 et PWM1.3
	LPC_PWM1->MR0 = 25E6/60E3; // fréquence de 60KHz
  LPC_PWM1->LER |= 1; // Mise à jour du MR0 chez PWM	
	LPC_PWM1->MR2 = 0; // 0% de rapport cyclique, signal PWM1.2 mis à 0
	LPC_PWM1->MR3 = 0; // 0% de rapport cyclique, signal PWM1.3 désactivé
	LPC_PWM1->MCR |= 2; // Reset sur la période
	LPC_PWM1->TCR |= 1 << 3; // Activation du mode PWM
}
void init_GPIO() {
	LPC_PINCON->PINSEL0 &= ~(3 << 4); // ligne New en GPIO sur P0.2
	LPC_GPIO0->FIODIR &= ~(1 << 2); // ligne New en entrée sur P0.2
	LPC_GPIO0->FIOPIN |= 1 << 2; // mise à 1 de P0.2, pas de nouveau échantillon
	LPC_PINCON->PINSEL1 &= ~( 0xFF ); // P0.16->P0.23 en GPIO
	LPC_GPIO0->FIODIR &= ~(0xFF << 16); // P0.16->P0.23 en entrée
	LPC_GPIO0->FIOPIN &= ~(0xFF << 16); // Mise à 0 de P0.16->P0.23
	
	LPC_PINCON->PINSEL4 &= ~(3<<12); // P2.6 en GPIO
	LPC_GPIO2->FIODIR |= 1 << 6;
	LPC_GPIO2->FIOPIN &= ~(1<<6);
	
}

void init_IR() {
	LPC_GPIOINT->IO0IntEnF |= 1 << 2; // interruption si on a un nouveau échantillon
	LPC_GPIOINT->IO0IntClr |= 1 << 2;
	NVIC_EnableIRQ(EINT2_IRQn);
}
int main() {
	init_GPIO();
	init_IR();
	init_PWM();
	init_TIMER0();
	init_TIMER2();
	
	while(1);
	
	return 0;
}
