#include <stdint.h>

/* Standard STM32L1xxx driver headers */
#include "stm32l1xx.h"

/* STM32L1xx Discovery Kit:
    - USER Pushbutton: connected to PA0 (GPIO Port A, PIN 0), CLK RCC_AHBENR_GPIOAEN
    - RESET Pushbutton: connected RESET
    - GREEN LED: connected to PB7 (GPIO Port B, PIN 7), CLK RCC_AHBENR_GPIOBEN 
    - BLUE LED: connected to PB6 (GPIO Port B, PIN 6), CLK RCC_AHBENR_GPIOBEN
    - Linear touch sensor/touchkeys: PA6, PA7 (group 2),  PC4, PC5 (group 9),  PB0, PB1 (group 3)
*/

/* stm32l1.h  line 815. GPIO_TypeDef pointer to GPIO_BASE address 0x40020400 */
#define GPIOB               ((GPIO_TypeDef *) 0x40020400)
//#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define RCC                 ((RCC_TypeDef *) RCC_BASE)
#define RCC_AHBENR_GPIOBEN  ((uint32_t)0x00000002)        /*!< GPIO port B clock enable */
#define TIM4                ((TIM_TypeDef *) TIM4_BASE)

void GPIO_Clock_Enable(){
		// Enable clock to GPIO port B
		//RCC->AHBENR	|= 0x00000002;
		RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
}

void GPIO_Pin_Init(){
		// Set pin 6 as general-purpose output
		GPIOB->MODER &= ~(0x03<<(2*6));		// Mode mask
		GPIOB->MODER |= 0x02<<(2*6);			// Set pin 6 as digital output
	
		// Set output type as push-pull
		GPIOB->OTYPER &= ~(1<<6);
	
		// Set pin 6 as alternative function 2 (TIM4)
		GPIOB->AFR[0] |= 0X2 << (4*6);
	
		// Set IO output speed
		GPIOB->OSPEEDR &= ~(0x03<<(2*6));
		GPIOB->OSPEEDR |= 0x03<<(2*6);
	
		// Set IO no pull-up pull-down
		GPIOB->PUPDR |= ~(0x00<<(2*6));	
}

void TIM4_Clock_Enable(){
		// Enable clock to TIM4
		RCC->APB1ENR	|= RCC_APB1ENR_TIM4EN;
}

int main(void){
		GPIO_Clock_Enable();
		GPIO_Pin_Init();
		//GPIOB->ODR |= (1<<6);	

		TIM4_Clock_Enable(); 
		TIM4->PSC		= 2097000/1000 - 1;		// Prescaler value
		TIM4->ARR		= 1000;								// Auto-reload value
		TIM4->CCR1	= 500;								// Compare and output register
		TIM4->CCMR1	= TIM_CCMR1_OC1M_0  |  TIM_CCMR1_OC1M_1;
		TIM4->CCER	= TIM_CCER_CC1E;			// Enable compare output 1
		TIM4->CR1		= TIM_CR1_CEN;				// Enable timer 4
		// Infinite loop
		while(1);
}
