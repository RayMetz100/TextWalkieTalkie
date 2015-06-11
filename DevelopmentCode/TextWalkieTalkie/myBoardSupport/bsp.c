/*****************************************************************************
* Product: DPP example, NUCLEO-L053R8 board, cooperative QV kernel
* Last Updated for Version: 5.4.0
* Date of the Last Update:  2015-04-05
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. state-machine.com.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* Web  : http://www.state-machine.com
* Email: info@state-machine.com
*****************************************************************************/
#include "qpc.h"
#include "blinky.h"
#include "bsp.h"

#include "stm32l0xx.h"  /* CMSIS-compliant header file for the MCU used */
/* add other drivers if necessary... */

Q_DEFINE_THIS_FILE

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
* DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
*/
enum KernelAwareISRs {
    GPIOPORTA_PRIO = QF_AWARE_ISR_CMSIS_PRI, /* see NOTE00 */
    GPIOPORTC_PRIO = QF_AWARE_ISR_CMSIS_PRI, /* see NOTE00 */
    SYSTICK_PRIO,
    /* ... */
    MAX_KERNEL_AWARE_CMSIS_PRI /* keep always last */
};
/* "kernel-aware" interrupts should not overlap the PendSV priority */
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >>(8-__NVIC_PRIO_BITS)));

void SysTick_Handler(void);

/* Local-scope defines -----------------------------------------------------*/
/* LED pins available on the board (just one user LED LD2--Green on PA.5) */
#define PORT_PIN_0  (1U << 0)
#define PORT_PIN_1  (1U << 1)
#define PORT_PIN_2  (1U << 2)
#define PORT_PIN_3  (1U << 3)
#define PORT_PIN_4  (1U << 4)
#define PORT_PIN_5  (1U << 5)
#define PORT_PIN_6  (1U << 6)
#define PORT_PIN_7  (1U << 7)
#define PORT_PIN_8  (1U << 8)
#define PORT_PIN_9  (1U << 9)
#define PORT_PIN_10  (1U << 10)
#define PORT_PIN_11  (1U << 11)
#define PORT_PIN_12  (1U << 12)
#define PORT_PIN_13  (1U << 13)
#define PORT_PIN_14  (1U << 14)
#define PORT_PIN_15  (1U << 15)

/* Button pins available on the board (just one user Button B1 on PC.13) */
//#define BTN_B1   (1U << 13)


static uint32_t l_rnd;  /* random seed */

#ifdef Q_SPY
    QSTimeCtr QS_tickTime_;
    QSTimeCtr QS_tickPeriod_;

    /* event-source identifiers used for tracing */
    static uint8_t const l_SysTick_Handler = 0U;

    enum AppRecords { /* application-specific trace records */
        PHILO_STAT = QS_USER
    };

#endif

/* ISRs used in the application ==========================================*/
void SysTick_Handler(void) {   /* system clock tick ISR */
    /* state of the button debouncing, see below */
    static struct ButtonsDebouncing {
        uint32_t depressed;
        uint32_t previous;
    } buttons = { ~0U, ~0U };
    uint32_t current;
    uint32_t tmp;

#ifdef Q_SPY
    {
        tmp = SysTick->CTRL; /* clear CTRL_COUNTFLAG */
        QS_tickTime_ += QS_tickPeriod_; /* account for the clock rollover */
    }
#endif

    QF_TICK_X(0U, &l_SysTick_Handler); /* process time events for rate 0 */

    /* get state of the user button */
    /* Perform the debouncing of buttons. The algorithm for debouncing
    * adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    * and Michael Barr, page 71.
    */
    current = ~GPIOC->IDR; /* read Port C with the state of Button B1 */
    tmp = buttons.depressed; /* save the debounced depressed buttons */
    buttons.depressed |= (buttons.previous & current); /* set depressed */
    buttons.depressed &= (buttons.previous | current); /* clear released */
    buttons.previous   = current; /* update the history */
    tmp ^= buttons.depressed;     /* changed debounced depressed */
}

/* BSP functions ===========================================================*/
void BSP_init(void) {
    /* NOTE: SystemInit() already called from the startup code
    *  but SystemCoreClock needs to be updated
    */
    SystemCoreClockUpdate();

	
	
    /* enable GPIOC clock port */
    RCC->IOPENR |= (1U << 2);

    /* configure LED (PC.0) pin as push-pull output, no pull-up, pull-down */
    GPIOC->MODER   &= ~((3U << 2*0));
    GPIOC->MODER   |=  ((1U << 2*0));
    GPIOC->OTYPER  &= ~((1U <<   0));
    GPIOC->OSPEEDR &= ~((3U << 2*0));
    GPIOC->OSPEEDR |=  ((1U << 2*0));
    GPIOC->PUPDR   &= ~((3U << 2*0));

    /* configure LED (PC.1) pin as push-pull output, no pull-up, pull-down */
    GPIOC->MODER   &= ~((3U << 2*1)); // 00s out the bits that were there.  keeps all others
    GPIOC->MODER   |=  ((1U << 2*1)); // 01: General purpose output mode
    GPIOC->OTYPER  &= ~((1U <<   1)); // set type = 0: Output push-pull (reset state)
    GPIOC->OSPEEDR &= ~((3U << 2*1)); // 0 out the bits that were there.  (00: Very low speed)
    GPIOC->OSPEEDR |=  ((1U << 2*1)); // 01: Low speeed
    GPIOC->PUPDR   &= ~((3U << 2*1)); // 0 out the bits.  00: No pull-up, pull-down

    /* configure LED (PC.2) pin as push-pull output, no pull-up, pull-down */
    GPIOC->MODER   &= ~((3U << 2*2)); // 00s out the bits that were there.  keeps all others
    GPIOC->MODER   |=  ((1U << 2*2)); // 01: General purpose output mode
    GPIOC->OTYPER  &= ~((1U <<   2)); // set type = 0: Output push-pull (reset state)
    GPIOC->OSPEEDR &= ~((3U << 2*2)); // 0 out the bits that were there.  (00: Very low speed)
    GPIOC->OSPEEDR |=  ((1U << 2*2)); // 01: Low speeed
    GPIOC->PUPDR   &= ~((3U << 2*2)); // 0 out the bits.  00: No pull-up, pull-down

    /* configure LED (PC.3) pin as push-pull output, no pull-up, pull-down */
    GPIOC->MODER   &= ~((3U << 2*3)); // 00s out the bits that were there.  keeps all others
    GPIOC->MODER   |=  ((1U << 2*3)); // 01: General purpose output mode
    GPIOC->OTYPER  &= ~((1U <<   3)); // set type = 0: Output push-pull (reset state)
    GPIOC->OSPEEDR &= ~((3U << 2*3)); // 0 out the bits that were there.  (00: Very low speed)
    GPIOC->OSPEEDR |=  ((1U << 2*3)); // 01: Low speeed
    GPIOC->PUPDR   &= ~((3U << 2*3)); // 0 out the bits.  00: No pull-up, pull-down

    /* configure LED (PC.4) pin as push-pull output, no pull-up, pull-down */
    GPIOC->MODER   &= ~((3U << 2*4)); // 00s out the bits that were there.  keeps all others
    GPIOC->MODER   |=  ((1U << 2*4)); // 01: General purpose output mode
    GPIOC->OTYPER  &= ~((1U <<   4)); // set type = 0: Output push-pull (reset state)
    GPIOC->OSPEEDR &= ~((3U << 2*4)); // 0 out the bits that were there.  (00: Very low speed)
    GPIOC->OSPEEDR |=  ((1U << 2*4)); // 01: Low speeed
    GPIOC->PUPDR   &= ~((3U << 2*4)); // 0 out the bits.  00: No pull-up, pull-down

    /* configure LED (PC.5) pin as push-pull output, no pull-up, pull-down */
    GPIOC->MODER   &= ~((3U << 2*5)); // 00s out the bits that were there.  keeps all others
    GPIOC->MODER   |=  ((1U << 2*5)); // 01: General purpose output mode
    GPIOC->OTYPER  &= ~((1U <<   5)); // set type = 0: Output push-pull (reset state)
    GPIOC->OSPEEDR &= ~((3U << 2*5)); // 0 out the bits that were there.  (00: Very low speed)
    GPIOC->OSPEEDR |=  ((1U << 2*5)); // 01: Low speeed
    GPIOC->PUPDR   &= ~((3U << 2*5)); // 0 out the bits.  00: No pull-up, pull-down

    /* configure LED (PC.6) pin as push-pull output, no pull-up, pull-down */
    GPIOC->MODER   &= ~((3U << 2*6)); // 00s out the bits that were there.  keeps all others
    GPIOC->MODER   |=  ((1U << 2*6)); // 01: General purpose output mode
    GPIOC->OTYPER  &= ~((1U <<   6)); // set type = 0: Output push-pull (reset state)
    GPIOC->OSPEEDR &= ~((3U << 2*6)); // 0 out the bits that were there.  (00: Very low speed)
    GPIOC->OSPEEDR |=  ((1U << 2*6)); // 01: Low speeed
    GPIOC->PUPDR   &= ~((3U << 2*6)); // 0 out the bits.  00: No pull-up, pull-down

    /* configure LED (PC.7) pin as push-pull output, no pull-up, pull-down */
    GPIOC->MODER   &= ~((3U << 2*7)); // 00s out the bits that were there.  keeps all others
    GPIOC->MODER   |=  ((1U << 2*7)); // 01: General purpose output mode
    GPIOC->OTYPER  &= ~((1U <<   7)); // set type = 0: Output push-pull (reset state)
    GPIOC->OSPEEDR &= ~((3U << 2*7)); // 0 out the bits that were there.  (00: Very low speed)
    GPIOC->OSPEEDR |=  ((1U << 2*7)); // 01: Low speeed
    GPIOC->PUPDR   &= ~((3U << 2*7)); // 0 out the bits.  00: No pull-up, pull-down

    /* configure LED (PC.8) pin as push-pull output, no pull-up, pull-down */
    GPIOC->MODER   &= ~((3U << 2*8)); // 00s out the bits that were there.  keeps all others
    GPIOC->MODER   |=  ((1U << 2*8)); // 01: General purpose output mode
    GPIOC->OTYPER  &= ~((1U <<   8)); // set type = 0: Output push-pull (reset state)
    GPIOC->OSPEEDR &= ~((3U << 2*8)); // 0 out the bits that were there.  (00: Very low speed)
    GPIOC->OSPEEDR |=  ((1U << 2*8)); // 01: Low speeed
    GPIOC->PUPDR   &= ~((3U << 2*8)); // 0 out the bits.  00: No pull-up, pull-down

    /* configure LED (PC.9) pin as push-pull output, no pull-up, pull-down */
    GPIOC->MODER   &= ~((3U << 2*9)); // 00s out the bits that were there.  keeps all others
    GPIOC->MODER   |=  ((1U << 2*9)); // 01: General purpose output mode
    GPIOC->OTYPER  &= ~((1U <<   9)); // set type = 0: Output push-pull (reset state)
    GPIOC->OSPEEDR &= ~((3U << 2*9)); // 0 out the bits that were there.  (00: Very low speed)
    GPIOC->OSPEEDR |=  ((1U << 2*9)); // 01: Low speeed
    GPIOC->PUPDR   &= ~((3U << 2*9)); // 0 out the bits.  00: No pull-up, pull-down

    /* configure LED (PC.10) pin as push-pull output, no pull-up, pull-down */
    GPIOC->MODER   &= ~((3U << 2*10)); // 00s out the bits that were there.  keeps all others
    GPIOC->MODER   |=  ((1U << 2*10)); // 01: General purpose output mode
    GPIOC->OTYPER  &= ~((1U <<   10)); // set type = 0: Output push-pull (reset state)
    GPIOC->OSPEEDR &= ~((3U << 2*10)); // 0 out the bits that were there.  (00: Very low speed)
    GPIOC->OSPEEDR |=  ((1U << 2*10)); // 01: Low speeed
    GPIOC->PUPDR   &= ~((3U << 2*10)); // 0 out the bits.  00: No pull-up, pull-down

    /* configure LED (PC.11) pin as push-pull output, no pull-up, pull-down */
    GPIOC->MODER   &= ~((3U << 2*11)); // 00s out the bits that were there.  keeps all others
    GPIOC->MODER   |=  ((1U << 2*11)); // 01: General purpose output mode
    GPIOC->OTYPER  &= ~((1U <<   11)); // set type = 0: Output push-pull (reset state)
    GPIOC->OSPEEDR &= ~((3U << 2*11)); // 0 out the bits that were there.  (00: Very low speed)
    GPIOC->OSPEEDR |=  ((1U << 2*11)); // 01: Low speeed
    GPIOC->PUPDR   &= ~((3U << 2*11)); // 0 out the bits.  00: No pull-up, pull-down

    /* configure LED (PC.12) pin as push-pull output, no pull-up, pull-down */
    GPIOC->MODER   &= ~((3U << 2*12)); // 00s out the bits that were there.  keeps all others
    GPIOC->MODER   |=  ((1U << 2*12)); // 01: General purpose output mode
    GPIOC->OTYPER  &= ~((1U <<   12)); // set type = 0: Output push-pull (reset state)
    GPIOC->OSPEEDR &= ~((3U << 2*12)); // 0 out the bits that were there.  (00: Very low speed)
    GPIOC->OSPEEDR |=  ((1U << 2*12)); // 01: Low speeed
    GPIOC->PUPDR   &= ~((3U << 2*12)); // 0 out the bits.  00: No pull-up, pull-down

    /* enable GPIOB clock port */
    RCC->IOPENR |= (1U << 1);

    /* configure Button Row 1-4 output (PB.0) pin as push-pull output, no pull-up, pull-down */
    GPIOB->MODER   &= ~((3U << 2*0)); // clear. 00 out the bits that were there.  (00s out the bits that were there.  keeps all others (not reset state)
    GPIOB->MODER   |=  ((1U << 2*0)); // 01: General purpose output mode
		
    GPIOB->OTYPER  &= ~((1U <<   0)); // clear. 0  out the bits that were there.  (0: Output push-pull, reset state)
    //GPIOB->OTYPER  |=  ((1U <<   0)); // set type = 1: Output open drain
		
    GPIOB->OSPEEDR &= ~((3U << 2*0)); // clear. 00 out the bits that were there.  (00: Very low speed) unknown reset state
    GPIOB->OSPEEDR |=  ((1U << 2*0)); // 01: Low speeed
		
    GPIOB->PUPDR   &= ~((3U << 2*0)); // clear. 00 out the bits that were there.  (00: No pull-up, pull-down) unknown reset state
    //GPIOB->PUPDR   |=  ((1U << 2*0)); // Output-pull-up 01


    /* Configure Button column S1,S13 as input, (PB.4), no pull-up, pull-down */
    GPIOB->MODER   &= ~(3UL << 2*4);  //clear+- the mode  00: Input mode
    GPIOB->OSPEEDR &= ~(3UL << 2*4);  // clear the speed
    GPIOB->OSPEEDR |=  (1UL << 2*4);  // speed = 01
    GPIOB->PUPDR   &= ~(3UL << 2*4);  // clear the pupd.  00: No pull-up, pull-down
    //GPIOB->PUPDR   |=  (2UL << 2*4);  // Input-pull-down 10






//    BSP_randomSeed(1234U); /* seed the random number generator */

    /* initialize the QS software tracing... */
    if (QS_INIT((void *)0) == 0) {
        Q_ERROR();
    }
    QS_OBJ_DICTIONARY(&l_SysTick_Handler);
}
/*..........................................................................*/
void BSP_ledAOff() {GPIOC->BSRR |= (PORT_PIN_8      );}  /* turn LED off */
void BSP_ledAOn () {GPIOC->BSRR |= (PORT_PIN_8 << 16);}  /* turn LED on  */
void BSP_ledBOff() {GPIOC->BSRR |= (PORT_PIN_9      );}  /* turn LED off */
void BSP_ledBOn () {GPIOC->BSRR |= (PORT_PIN_9 << 16);}  /* turn LED on  */
void BSP_ledCOff() {GPIOC->BSRR |= (PORT_PIN_10      );}  /* turn LED off */
void BSP_ledCOn () {GPIOC->BSRR |= (PORT_PIN_10 << 16);}  /* turn LED on  */
void BSP_ledDOff() {GPIOC->BSRR |= (PORT_PIN_11      );}  /* turn LED off */
void BSP_ledDOn () {GPIOC->BSRR |= (PORT_PIN_11 << 16);}  /* turn LED on  */
void BSP_ledEOff() {GPIOC->BSRR |= (PORT_PIN_12      );}  /* turn LED off */
void BSP_ledEOn () {GPIOC->BSRR |= (PORT_PIN_12 << 16);}  /* turn LED on  */

void BSP_led0Off() {GPIOC->BSRR |= (PORT_PIN_0 << 16);}  /* turn LED off */
void BSP_led0On () {GPIOC->BSRR |=  PORT_PIN_0       ;}  /* turn LED on  */
void BSP_led1Off() {GPIOC->BSRR |= (PORT_PIN_1 << 16);}  /* turn LED off */
void BSP_led1On () {GPIOC->BSRR |=  PORT_PIN_1       ;}  /* turn LED on  */
void BSP_led2Off() {GPIOC->BSRR |= (PORT_PIN_2 << 16);}  /* turn LED off */
void BSP_led2On () {GPIOC->BSRR |=  PORT_PIN_2       ;}  /* turn LED on  */
void BSP_led3Off() {GPIOC->BSRR |= (PORT_PIN_3 << 16);}  /* turn LED off */
void BSP_led3On () {GPIOC->BSRR |=  PORT_PIN_3       ;}  /* turn LED on  */
void BSP_led4Off() {GPIOC->BSRR |= (PORT_PIN_4 << 16);}  /* turn LED off */
void BSP_led4On () {GPIOC->BSRR |=  PORT_PIN_4       ;}  /* turn LED on  */
void BSP_led5Off() {GPIOC->BSRR |= (PORT_PIN_5 << 16);}  /* turn LED off */
void BSP_led5On () {GPIOC->BSRR |=  PORT_PIN_5       ;}  /* turn LED on  */
void BSP_led6Off() {GPIOC->BSRR |= (PORT_PIN_6 << 16);}  /* turn LED off */
void BSP_led6On () {GPIOC->BSRR |=  PORT_PIN_6       ;}  /* turn LED on  */
void BSP_led7Off() {GPIOC->BSRR |= (PORT_PIN_7 << 16);}  /* turn LED off */
void BSP_led7On () {GPIOC->BSRR |=  PORT_PIN_7       ;}  /* turn LED on  */

// button outputs
//Push-pull mode: A “0” in the Output register activates the N-MOS whereas a “1” in
//the Output register activates the P-MOS
void BSP_buttonR0Off(){GPIOB->BSRR |= (PORT_PIN_0 << 16);} // blow it off the variable to the left.  leave all 0s?
void BSP_buttonR0On (){GPIOB->BSRR |= (PORT_PIN_0      );}

int BSP_buttonCS1()
{
	int a = 0;
	int r = 0;
	r = GPIOB->IDR;
	a = r & PORT_PIN_4;
	
	if (a == PORT_PIN_4)
	  return 1; // button pressed
  else
	  return 0; // button not pressed
};
//int BSP_buttonCS2(void);
//int BSP_buttonCS3(void);
//int BSP_buttonCS4(void);


//#define BTN_B1   (1U << 13)

int myButton(void)
{
	int a = 0;
	int r = 0;
	a =  GPIOB->IDR;
	r = (a & PORT_PIN_0);
	
  if (r == PORT_PIN_0)
	{
	  return 1; // button pressed
	}
  else
	{
	  return 0; // button not pressed
	}
}

#define UNBOUNCE_CNT        5                     // unbounce the keys

int S2Pressed (void) {
  static int S2KeyCount = 0, S2KeyPressed = 0;

  if (S2KeyPressed) {
    if (!((GPIOA->IDR & PORT_PIN_6) == PORT_PIN_6 )) {             // Check if S2 is not pressed
      if (S2KeyCount < UNBOUNCE_CNT) S2KeyCount++;
      else {
        S2KeyPressed = 0;
        S2KeyCount = 0;    
      }
    }
  }
  else {
    if (((GPIOA->IDR & PORT_PIN_6) == 0 ))  {             // Check if S2 is pressed
      if (S2KeyCount < UNBOUNCE_CNT) S2KeyCount++;
      else {
        S2KeyPressed = 1;
        S2KeyCount = 0;
		return (1);
      }
    }
  }
  return (0);
}



/*..........................................................................*/
//void BSP_displayPhilStat(uint8_t n, char const *stat) {
//    if (stat[0] == 'h') {
//        GPIOA->BSRR |= PORT_PIN_5;  /* turn LED on  */
//    }
//    else {
//        GPIOA->BSRR |= (PORT_PIN_5 << 16);  /* turn LED off */
//    }

//    QS_BEGIN(PHILO_STAT, AO_Philo[n]) /* application-specific record begin */
//        QS_U8(1, n);                  /* Philosopher number */
//        QS_STR(stat);                 /* Philosopher status */
//    QS_END()
//}
/*..........................................................................*/
//void BSP_displayPaused(uint8_t paused) {
//    /* not enough LEDs to implement this feature */
//    if (paused != (uint8_t)0) {
//        //GPIOA->BSRR |= (LED_LD2);  /* turn LED[n] on  */
//    }
//    else {
//        //GPIOA->BSRR |= (LED_LD2 << 16);  /* turn LED[n] off */
//    }
//}
/*..........................................................................*/
uint32_t BSP_random(void) { /* a very cheap pseudo-random-number generator */
    /* "Super-Duper" Linear Congruential Generator (LCG)
    * LCG(2^32, 3*7*11*13*23, 0, seed)
    */
    l_rnd = l_rnd * (3U*7U*11U*13U*23U);
    return l_rnd >> 8;
}
/*..........................................................................*/
void BSP_randomSeed(uint32_t seed) {
    l_rnd = seed;
}
/*..........................................................................*/
void BSP_terminate(int16_t result) {
    (void)result;
}

/* QF callbacks ============================================================*/
void QF_onStartup(void) {
    /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* set priorities of ALL ISRs used in the system, see NOTE00
    *
    * !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    */
    NVIC_SetPriority(SysTick_IRQn,   SYSTICK_PRIO);
    /* ... */

    /* enable IRQs... */
}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QV_onIdle(void) {  /* called with interrupts disabled, see NOTE01 */

    /* toggle an LED on and then off (not enough LEDs, see NOTE02) */
    //GPIOA->BSRR |= (LED_LD2);        /* turn LED[n] on  */
    //GPIOA->BSRR |= (LED_LD2 << 16);  /* turn LED[n] off */

#ifdef Q_SPY
    QF_INT_ENABLE();
    if ((USART2->ISR & 0x0080U) != 0) {  /* is TXE empty? */
        uint16_t b;

        QF_INT_DISABLE();
        b = QS_getByte();
        QF_INT_ENABLE();

        if (b != QS_EOD) {  /* not End-Of-Data? */
            USART2->TDR  = (b & 0xFFU);  /* put into the DR register */
        }
    }
#elif defined NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M MCU.
    */
    /* !!!CAUTION!!!
    * The QF_CPU_SLEEP() contains the WFI instruction, which stops the CPU
    * clock, which unfortunately disables the JTAG port, so the ST-Link
    * debugger can no longer connect to the board. For that reason, the call
    * to QF_CPU_SLEEP() has to be used with CAUTION.
    */
    /* NOTE: If you find your board "frozen" like this, strap BOOT0 to VDD and
    * reset the board, then connect with ST-Link Utilities and erase the part.
    * The trick with BOOT(0) is it gets the part to run the System Loader
    * instead of your broken code. When done disconnect BOOT0, and start over.
    */
    //QV_CPU_SLEEP();  /* atomically go to sleep and enable interrupts */
    QF_INT_ENABLE(); /* for now, just enable interrupts */
#else
    QF_INT_ENABLE(); /* just enable interrupts */
#endif
}

/*..........................................................................*/
/* NOTE Q_onAssert() defined in assembly in startup_<device>.s */

/* QS callbacks ============================================================*/
#ifdef Q_SPY
/*..........................................................................*/
#define __DIV(__PCLK, __BAUD)       (((__PCLK / 4) *25)/(__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   \
    (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) \
        * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) \
    ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

/*..........................................................................*/
uint8_t QS_onStartup(void const *arg) {
    static uint8_t qsBuf[2*1024]; /* buffer for Quantum Spy */

    (void)arg; /* avoid the "unused parameter" compiler warning */
    QS_initBuf(qsBuf, sizeof(qsBuf));

    /* enable peripheral clock for USART2 */
    RCC->IOPENR  |= ( 1ul <<  0);   /* Enable GPIOA clock   */
    RCC->APB1ENR |= ( 1ul << 17);   /* Enable USART#2 clock */

    /* Configure PA3 to USART2_RX, PA2 to USART2_TX */
    GPIOA->AFR[0] &= ~((15ul << 4* 3) | (15ul << 4* 2) );
    GPIOA->AFR[0] |=  (( 4ul << 4* 3) | ( 4ul << 4* 2) );
    GPIOA->MODER  &= ~(( 3ul << 2* 3) | ( 3ul << 2* 2) );
    GPIOA->MODER  |=  (( 2ul << 2* 3) | ( 2ul << 2* 2) );

    USART2->BRR  = __USART_BRR(SystemCoreClock, 115200ul);  /* baud rate */
    USART2->CR3  = 0x0000;         /* no flow control */
    USART2->CR2  = 0x0000;         /* 1 stop bit      */
    USART2->CR1  = ((1ul <<  2) |  /* enable RX       */
                    (1ul <<  3) |  /* enable TX       */
                    (0ul << 12) |  /* 8 data bits     */
                    (0ul << 28) |  /* 8 data bits     */
                    (1ul <<  0) ); /* enable USART    */

    QS_tickPeriod_ = SystemCoreClock / BSP_TICKS_PER_SEC;
    QS_tickTime_ = QS_tickPeriod_; /* to start the timestamp at zero */

    /* setup the QS filters... */
    QS_FILTER_ON(QS_QEP_STATE_ENTRY);
    QS_FILTER_ON(QS_QEP_STATE_EXIT);
    QS_FILTER_ON(QS_QEP_STATE_INIT);
    QS_FILTER_ON(QS_QEP_INIT_TRAN);
    QS_FILTER_ON(QS_QEP_INTERN_TRAN);
    QS_FILTER_ON(QS_QEP_TRAN);
    QS_FILTER_ON(QS_QEP_IGNORED);
    QS_FILTER_ON(QS_QEP_DISPATCH);
    QS_FILTER_ON(QS_QEP_UNHANDLED);

//    QS_FILTER_ON(QS_QF_ACTIVE_ADD);
//    QS_FILTER_ON(QS_QF_ACTIVE_REMOVE);
//    QS_FILTER_ON(QS_QF_ACTIVE_SUBSCRIBE);
//    QS_FILTER_ON(QS_QF_ACTIVE_UNSUBSCRIBE);
//    QS_FILTER_ON(QS_QF_ACTIVE_POST_FIFO);
//    QS_FILTER_ON(QS_QF_ACTIVE_POST_LIFO);
//    QS_FILTER_ON(QS_QF_ACTIVE_GET);
//    QS_FILTER_ON(QS_QF_ACTIVE_GET_LAST);
//    QS_FILTER_ON(QS_QF_EQUEUE_INIT);
//    QS_FILTER_ON(QS_QF_EQUEUE_POST_FIFO);
//    QS_FILTER_ON(QS_QF_EQUEUE_POST_LIFO);
//    QS_FILTER_ON(QS_QF_EQUEUE_GET);
//    QS_FILTER_ON(QS_QF_EQUEUE_GET_LAST);
//    QS_FILTER_ON(QS_QF_MPOOL_INIT);
//    QS_FILTER_ON(QS_QF_MPOOL_GET);
//    QS_FILTER_ON(QS_QF_MPOOL_PUT);
//    QS_FILTER_ON(QS_QF_PUBLISH);
//    QS_FILTER_ON(QS_QF_RESERVED8);
//    QS_FILTER_ON(QS_QF_NEW);
//    QS_FILTER_ON(QS_QF_GC_ATTEMPT);
//    QS_FILTER_ON(QS_QF_GC);
    QS_FILTER_ON(QS_QF_TICK);
//    QS_FILTER_ON(QS_QF_TIMEEVT_ARM);
//    QS_FILTER_ON(QS_QF_TIMEEVT_AUTO_DISARM);
//    QS_FILTER_ON(QS_QF_TIMEEVT_DISARM_ATTEMPT);
//    QS_FILTER_ON(QS_QF_TIMEEVT_DISARM);
//    QS_FILTER_ON(QS_QF_TIMEEVT_REARM);
//    QS_FILTER_ON(QS_QF_TIMEEVT_POST);
//    QS_FILTER_ON(QS_QF_TIMEEVT_CTR);
//    QS_FILTER_ON(QS_QF_CRIT_ENTRY);
//    QS_FILTER_ON(QS_QF_CRIT_EXIT);
//    QS_FILTER_ON(QS_QF_ISR_ENTRY);
//    QS_FILTER_ON(QS_QF_ISR_EXIT);
//    QS_FILTER_ON(QS_QF_INT_DISABLE);
//    QS_FILTER_ON(QS_QF_INT_ENABLE);
//    QS_FILTER_ON(QS_QF_ACTIVE_POST_ATTEMPT);
//    QS_FILTER_ON(QS_QF_EQUEUE_POST_ATTEMPT);
//    QS_FILTER_ON(QS_QF_MPOOL_GET_ATTEMPT);
//    QS_FILTER_ON(QS_QF_RESERVED1);
//    QS_FILTER_ON(QS_QF_RESERVED0);

//    QS_FILTER_ON(QS_QK_MUTEX_LOCK);
//    QS_FILTER_ON(QS_QK_MUTEX_UNLOCK);
//    QS_FILTER_ON(QS_QK_SCHEDULE);
//    QS_FILTER_ON(QS_QK_RESERVED1);
//    QS_FILTER_ON(QS_QK_RESERVED0);

//    QS_FILTER_ON(QS_QEP_TRAN_HIST);
//    QS_FILTER_ON(QS_QEP_TRAN_EP);
//    QS_FILTER_ON(QS_QEP_TRAN_XP);
//    QS_FILTER_ON(QS_QEP_RESERVED1);
//    QS_FILTER_ON(QS_QEP_RESERVED0);

    QS_FILTER_ON(QS_SIG_DICT);
    QS_FILTER_ON(QS_OBJ_DICT);
    QS_FILTER_ON(QS_FUN_DICT);
    QS_FILTER_ON(QS_USR_DICT);
    QS_FILTER_ON(QS_EMPTY);
    QS_FILTER_ON(QS_RESERVED3);
    QS_FILTER_ON(QS_RESERVED2);
    QS_FILTER_ON(QS_TEST_RUN);
    QS_FILTER_ON(QS_TEST_FAIL);
    QS_FILTER_ON(QS_ASSERT_FAIL);

    return (uint8_t)1; /* return success */
}
/*..........................................................................*/
void QS_onCleanup(void) {
}
/*..........................................................................*/
QSTimeCtr QS_onGetTime(void) { /* NOTE: invoked with interrupts DISABLED */
    if ((SysTick->CTRL & 0x00010000) == 0) {  /* COUNT no set? */
        return QS_tickTime_ - (QSTimeCtr)SysTick->VAL;
    }
    else { /* the rollover occured, but the SysTick_ISR did not run yet */
        return QS_tickTime_ + QS_tickPeriod_ - (QSTimeCtr)SysTick->VAL;
    }
}
/*..........................................................................*/
void QS_onFlush(void) {
    uint16_t b;

    QF_INT_DISABLE();
    while ((b = QS_getByte()) != QS_EOD) {    /* while not End-Of-Data... */
        QF_INT_ENABLE();
        while ((USART2->ISR & 0x0080U) == 0U) { /* while TXE not empty */
        }
        USART2->TDR  = (b & 0xFFU);  /* put into the DR register */
    }
    QF_INT_ENABLE();
}
#endif /* Q_SPY */
/*--------------------------------------------------------------------------*/

/*****************************************************************************
* NOTE00:
* The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
* ISR priority that is disabled by the QF framework. The value is suitable
* for the NVIC_SetPriority() CMSIS function.
*
* Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
* with the numerical values of priorities equal or higher than
* QF_AWARE_ISR_CMSIS_PRI) are allowed to call any QF services. These ISRs
* are "QF-aware".
*
* Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
* level (i.e., with the numerical values of priorities less than
* QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
* Such "QF-unaware" ISRs cannot call any QF services. The only mechanism
* by which a "QF-unaware" ISR can communicate with the QF framework is by
* triggering a "QF-aware" ISR, which can post/publish events.
*
* NOTE01:
* The QV_onIdle() callback is called with interrupts disabled, because the
* determination of the idle condition might change by any interrupt posting
* an event. QV_onIdle() must internally enable interrupts, ideally
* atomically with putting the CPU to the power-saving mode.
*
* NOTE02:
* The User LED is used to visualize the idle loop activity. The brightness
* The User LED is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*/
