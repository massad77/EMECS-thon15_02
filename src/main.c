/**************************************************************************//**
 * @file
 * @brief Empty Project
 * @author Energy Micro AS
 * @version 3.20.2
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See 
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"  
 * for details. Before using this software for any purpose, you must agree to the 
 * terms of that agreement.
 *
 ******************************************************************************/
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "uart.h"
#include "motor.h"
#include "em_gpio.h"
#include "em_usart.h"

#include "bsp.h"

volatile uint32_t msTicks; /* counts 1ms timeTicks */

void Delay(uint32_t dlyTicks);

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       /* increment counter necessary in Delay()*/
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  char buffer[] = "AT";
  uint8_t delay_type = 0;

  /* Chip errata */
  CHIP_Init();

  /* initalize clocks */
  CMU->CTRL |= (1 << 14);                         // Set HF clock divider to /2 to keep core frequency <32MHz
  CMU->OSCENCMD |= 0x4;                           // Enable XTAL Oscillator
  while(! (CMU->STATUS & 0x8) );                  // Wait for XTAL osc to stabilize
  CMU->CMD = 0x2;                                 // Select HF XTAL osc as system clock source. 48MHz XTAL, but we divided the system clock by 2, therefore our HF clock should be 24MHz

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;

  usart_init();
  motor_init();

  usart_enable_rx_isr();

  BSP_LedsInit();
  BSP_LedSet(0);
  BSP_LedSet(1);

  usart_send_string(buffer);

  /* Infinite loop */
  while (1) {
	  BSP_LedToggle(1);

	  switch(rx_data){
	    case 'f':
		  Move_Forward();
		  delay_type = 1;
		  break;
	    case 'b':
		  Move_Backward();
		  delay_type = 1;
		  break;
	    case 'r':
		  Move_Left();
		  delay_type = 2;
		  break;
	    case 'l':
		  Move_Right();
		  delay_type = 2;
		  break;
	    default:
	      delay_type = 1;
		  Stop_Robot();
		  break;
	  }

	  rx_data = 0;

	  //Chooses the delay depending on the movement
	  if (delay_type == 1){
		  Delay(1000);
	  }else if(delay_type == 2){
		  Delay(200);
	  }

  }
}
