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
#define GPIO_AFRL_AFRL2     ((uint32_t)0x00000F00)
#define GPIO_AF2            0x2

void GPIO_Clock_Enable(){
		// Enable clock to GPIO port B
		//RCC->AHBENR	|= 0x00000002;
		RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
}

void GPIO_Pin_Init(){
		/* MODER6[1:0] involving bits 12 & 13. 0b0010(0x02): Alternate function mode */
		GPIOB->MODER &= ~(0x03 << 12);			// Clear mode bits
		GPIOB->MODER |= 0x02 << 12;					// Set pin PB.6 as alternate function
	
		// Set pin 6 as alternative function 2 (TIM4). bit y = 6
		// Bits 31:0 AFRLy: Alternate function selection for port x bit y (y = 0..7)
		// AFRLy selection: 0010: AF2 => 0x2 pages 176 & 187, figure 20 rm0038
		// #define GPIO_AFR(n, af)     ((af) << ((n) * 4)) n pin number af alternate function AF2
    // #define GPIO_AFR_MASK(n)     (0xf << ((n) * 4))
		// libopencm3 free/libre/open-source firmware library for various ARM Cortex-M3 microcontrollers.
		/*  #define GPIO_AF0                        0x0
  249 #define GPIO_AF1                        0x1
  250 #define GPIO_AF2                        0x2
  251 #define GPIO_AF3                        0x3
  252 #define GPIO_AF4                        0x4
  253 #define GPIO_AF5                        0x5
  254 #define GPIO_AF6                        0x6
  255 #define GPIO_AF7                        0x7
  256 #define GPIO_AF8                        0x8
  257 #define GPIO_AF9                        0x9
  258 #define GPIO_AF10                       0xa
  259 #define GPIO_AF11                       0xb
  260 #define GPIO_AF12                       0xc
  261 #define GPIO_AF13                       0xd
  262 #define GPIO_AF14                       0xe
  263 #define GPIO_AF15                       0xf
		PB.6 selects AFRL6[3:0] from AFRL register & involves bits 24 to 27 rm0038. From stm32l152rb datasheet page 44 table 9,
		we choose AFIO2(PB6 alt func: TIM4_CH1) or 0010(0x02): AF2 alternate function. We borrow from libopencm3 define for AF2 */
		GPIOB->AFR[0] &= ~0xF << (24);			// Clear bits 24-27
		GPIOB->AFR[0] |= GPIO_AF2 << (24);	/* Set 0x02 mask alternate function. For TIM3 alternate function, AFRLy bit y=6, AF2 is
		                                       PC.6. Refer to page 44 of stm32l152rb datasheet and page 176 of rm0038. */
		
		GPIOB->OTYPER &= ~(0x1<<6);					// Set PB.6 pin as push-pull output type		
		
		// Set IO output speed
		GPIOB->OSPEEDR &= ~(0x03<<(2*6));
		GPIOB->OSPEEDR |= 0x03<<(2*6);
	
		// Set IO no pull-up pull-down
		GPIOB->PUPDR |= ~(0x00<<(2*6));	
}

void TIM4_Clock_Enable(){
		// Enable clock to TIM4
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
}

int main(void){
		GPIO_Clock_Enable();
		GPIO_Pin_Init();
		//GPIOB->ODR |= (1<<6);	

		TIM4_Clock_Enable();
	
//		TIM4->PSC		= 2097000/1000 - 1;		// Prescaler value
		//TIM4->PSC = 100;
//		TIM4->ARR		= 1000;								// Auto-reload value
//		TIM4->CCR1	= 500;								// Compare and output register
//		TIM4->CCMR1	= TIM_CCMR1_OC1M_0  |  TIM_CCMR1_OC1M_1;
//		TIM4->CCER	= TIM_CCER_CC1E;			// Enable compare output 1
//		TIM4->CR1		= TIM_CR1_CEN;				// Enable timer 4		
	
    /* Adapted from TIM3 channel 1 output trigger signal to ADC peripheral code that works. */
	  /* Output Compare PWM Mode. Timer 3 channel 1 output 1Hz. page 395, 402-403 rm0038.
	     1) Select counter clock(internal, external, and prescaler. Default: SMS=000 TIMx_SMCR register.  */
		TIM4->PSC = 2097000/1000 - 1;     /* MSI 2.097MHz is default internal clock. Timer 3 counter clock 1KHz, Prescaler value */
		/* 2) Write desired data in TIMx_ARR and TIMx_CCRx registers. */
	  TIM4->ARR = 1000-1;               /* Auto-reload value. Timer CNT count from zero to ARR value. */
    TIM4->CCR1 = 499;                 /* CCR1 selected from 4 channels CCRx by assigning value. Output compare register for channel 1.
	                                       Varies duty cycle if CCR1 != ARR, 499/999 is 50% duty cycle. */
	  /* 3) Set the CCxIE and/or CCxDE bits if an interrupt and/or a DMA request is to be generated.
    TIM4->DIER |= TIM_DIER_CC1IE; TIM4->DIER |= TIM_DIER_CC1DE; TIM4->DIER |= (1UL << 6);	TIM4->SR |= (1UL << 0);	*/
	  /* 4) Select output mode, PWM. */
	  TIM4->CCMR1 |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1; /* OC1M = 110 for PWM output mode 1 on channel 1 */
		TIM4->CCMR1	|= TIM_CCMR1_OC1PE;   /* Enable preload for channel 1 */
		TIM4->CR1 |= TIM_CR1_ARPE;        /* Auto-reload preload enable in ARR */
	  /* 5) Enable Output Enable Circuit, TIMx->CCER CC1E */
  	TIM4->CCER |= TIM_CCER_CC1E;      /* Capture/Compare 1 output enable */
	  TIM4->EGR |= TIM_EGR_UG;          /* Update Generation, re-initialize timer counter */
		/* 6) Enable the counter by setting the CEN bit in the TIMx_CR1 register. */
	  TIM4->CR1 |= TIM_CR1_CEN;         /* Enable timer 3/counter */

		// Infinite loop
		while(1);
}
