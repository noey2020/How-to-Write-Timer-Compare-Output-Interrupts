# How-to-Write-Timer-Compare-Output-Interrupts
Timer4 Compare Output Interrupt Toggle LED

Timer TIMx Basics	July 7, 2020

I appreciate comments. Shoot me an email at noel_s_cruz@yahoo.com!

Hire Me!

Continuing on from a previous post on Interrupts, https://github.com/noey2020/How-to-Write-Systick-Timer-CMSIS-Style,
we will tackle timers.

The general-purpose timers, TIM2 to TIM5, consist of a 16-bit or 32-bit auto-reload
counter driven by a programmable prescaler.

They may be used for a variety of purposes, including measuring the pulse lengths of 
input signals (input capture) or generating output waveforms (output compare and PWM).

Pulse lengths and waveform periods can be modulated from a few microseconds to several
milliseconds using the timer prescaler and the RCC clock controller prescalers.

The timers are completely independent, and do not share any resources. They can be
synchronized together.

Before first use, we need to configure them. Note that timer registers, bit definitions
might differ depending on how the timer is set as input or output. Consider TIMx 
capture/compare mode register 1 (TIMx_CCMR1). Bits 6:4 OC1M: Output compare 1 mode in 
output mode. It is different in input mode because it is now Bits 7:4 IC1F: Input 
capture 1 filter. The caveat is the same bit can have a different meaning for the input
stage and for the output stage.

The following shows in steps how to configure timer TIM4 as output compare, PWM. See 
step 3 on varying duty cycle.

1) Enable TIM4 clock.

   /* Configure channel 1 output of timer 4 used as trigger signal to ADC. Square wave output with frequency 1Hz and 50% duty cycle. */
   
   RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; /* Enable TIM4 clock */

2) Select counter clock(internal, external, and prescaler. Default: SMS=000 TIMx_SMCR register.  

   /* Output Compare PWM Mode. Timer 3 channel 1 output 1Hz. page 395, 402-403 rm0038. */
   
   TIM4->PSC = 2097000/1000 - 1;     /* MSI 2.097MHz is default internal clock. Timer 4 counter clock 1KHz, Prescaler value */

3) Write desired data in TIMx_ARR and TIMx_CCRx registers.

   TIM4->ARR = 1000-1;               /* Auto-reload value. Timer CNT count from zero to ARR value. */
   
   TIM4->CCR1 = 499;                 /* CCR1 selected from 4 channels CCRx by assigning value. Output compare register for channel 1.
   
	                                Varies duty cycle if CCR1 != ARR, 499/999 is 50% duty cycle. */
	                                
4) Set the CCxIE and/or CCxDE bits if an interrupt and/or a DMA request is to be generated.

   TIM4->DIER |= TIM_DIER_CC1IE; TIM4->DIER |= TIM_DIER_CC1DE; TIM4->DIER |= (1UL << 6); TIM4->SR |= (1UL << 0);
   
5) Select output mode, PWM.

   TIM4->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; /* OC1M = 110 for PWM output mode 1 on channel 1 */
   
   TIM4->CCMR1	|= TIM_CCMR1_OC1PE;   /* Enable preload for channel 1 */
   
   TIM4->CR1 |= TIM_CR1_ARPE;        /* Auto-reload preload enable in ARR */

6) Enable Output Enable Circuit, TIMx->CCER CC1E 

   TIM4->CCER |= TIM_CCER_CC1E;      /* Capture/Compare 1 output enable */
   
   TIM4->EGR |= TIM_EGR_UG;          /* Update Generation, re-initialize timer counter */
   
7) Enable the counter by setting the CEN bit in the TIMx_CR1 register.

   TIM4->CR1 |= TIM_CR1_CEN;         /* Enable timer 3/counter */

Similarly, these are steps for just output compare only.

    TIM4->PSC   = 2097000/1000 - 1; 
    
    TIM4->PSC   = 100;
    
    TIM4->ARR   = 1000;	
    
    TIM4->CCR1  = 500;	
    
    TIM4->CCMR1	= TIM_CCMR1_OC1M_0  |  TIM_CCMR1_OC1M_1;
    
    TIM4->CCER	= TIM_CCER_CC1E;
    
    TIM4->CR1	= TIM_CR1_CEN;
    
The next installment will touch on using timers to trigger ADC analog measurements and
digitize the. Here the link: https://github.com/noey2020/How-to-Write-Timer-Trigger-ADC-STM32L1.

I appreciate comments. Shoot me an email at noel_s_cruz@yahoo.com!

Hire Me!

Happy coding!

