# Fire Alarm
Configuring LPC1768 microcontroller to generate PWM signal for fans and alarm depending on temperature on Keil µVision IDE. Subject of an exercise in my microcontroller course at Sorbonne University.
# Exercise Criteria:
We have a temperature sensor that computes a value on 8 bits (connected to P0.16 to P0.23). When a new value is computed, a bit we call "new" (connected to P0.2) is reset to 0.

We want the following behavior: 
1. When the temperature is higher than 35°C, a fan is turned on, we generate a 60KHz PWM signal on P1.20 with a 50% duty cycle
2. When the temperature is higher than 45°C, a fan is turned on, we generate a 60KHz PWM signal on P1.20 *and* P1.23 with a 90% duty cycle
3. When the temperature is higher than 50°C, a fan is turned on, we generate a 60KHz PWM signal on P1.20 *and* P1.23 with a 90% duty cycle *and* launch an alarm for 30 seconds using a 15KHz PWM signal with a 50% duty cycle on P2.6.

If the temperature falls below 35°C, the system is stopped and put to sleep. Also, the third signal is not generated using PWM component, but using two timers. Do not ask why.
