/*
 * Timer0_delay.c
 *
 *  Created on: May 23, 2023
 *  Author: Vishal Shrivastava
 *
 */


#include "MAX7219_EFM8.h"



//-----------------------------------------------------------------------------
// T0_Wait_us
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) uint8_t counts - number of microseconds to wait
//                        range is full range of character: 0 to 255
//
// Configure Timer0 to wait for <us> microseconds using SYSCLK as its time
// base.
//
//-----------------------------------------------------------------------------

void delay_us (uint16_t counts)       //1 count = 4.5 us
{
  TCON &= ~0x30;                      // Stop Timer0; Clear TCON_TF0
  TMOD &= ~0x0f;                      // 16-bit free run mode
  TMOD |=  0x01;

  CKCON0 |= 0x04;                     // Timer0 counts SYSCLKs

  while(counts)
    {
      TCON_TR0 = 0;                    // Stop Timer0
      TH0 = ((-(SYSCLK/1000000)) >> 8);   // Overflow in 4.5us
      TL0 = ((-(SYSCLK/1000000)) & 0xFF);
      TCON_TF0 = 0;                    // Clear overflow indicator
      TCON_TR0 = 1;                    // Start Timer0
      while(!TCON_TF0);                // Wait for overflow
      counts--;                            // Update ms counter
    }

  TCON_TR0 = 0;                       // Stop Timer0
}


//-----------------------------------------------------------------------------
// T0_Wait_ms
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) uint8_t ms - number of milliseconds to wait
//                        range is full range of character: 0 to 255
//
// Configure Timer0 to wait for <ms> milliseconds using SYSCLK as its time
// base.
//
//-----------------------------------------------------------------------------
uint16_t delay_ms (uint16_t ms_counts)
{
  uint16_t ms = 0;

  ms = ms_counts;
  TCON &= ~0x30;                      // Stop Timer0; Clear TCON_TF0
  TMOD &= ~0x0f;                      // 16-bit free run mode
  TMOD |=  0x01;

  CKCON0 |= 0x04;                     // Timer0 counts SYSCLKs

  while(ms_counts)
    {
      TCON_TR0 = 0;                    // Stop Timer0
      TH0 = ((-(SYSCLK/1000)) >> 8);   // Overflow in 1ms
      TL0 = ((-(SYSCLK/1000)) & 0xFF);
      TCON_TF0 = 0;                    // Clear overflow indicator
      TCON_TR0 = 1;                    // Start Timer0
      while(!TCON_TF0);                // Wait for overflow
      ms_counts--;                            // Update ms counter
    }

  TCON_TR0 = 0;                       // Stop Timer0
  return ms;
}

//void delay_sec(uint)
//{
//
//}
