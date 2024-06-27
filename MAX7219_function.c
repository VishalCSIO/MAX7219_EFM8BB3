/*
 * MAX7219main.c
 *
 *  Created on: 16-Jun-2024
 *      Author: vishal Shrivastava
 *
 *==============================================================================
 *      To display characters on an eight 7-segment display using
 *      SPI communication with an EFM8BB3 Silicon Labs micro-controller,
 *      you can interface with a MAX7219 display driver. These drivers
 *      require only three pins for controlling up to eight 7-segment
 *      displays, simplifying the process significantly.
 *==============================================================================
 */



//==============================================================================
// Includes
//==============================================================================

#include "MAX7219_EFM8.h"

//==============================================================================
// Global Variables
//==============================================================================






//==============================================================================
// Support Sub Routines
//==============================================================================



///=============================================================================
/// SPI_transfer_data
///=============================================================================
//
// Return Value : None
// Parameters   : None
//
// Note: SPI_Data_Array must contain the data to be sent before calling this
// function.
//
// Writes an array of values of size MAX_BUFFER_SIZE to the SPI Slave.  The
// command consists of:
//
// Command = SPI_WRITE_BUFFER
// Length = 1 byte of command, MAX_BUFFER_SIZE bytes of data
//
// Note: Polled mode is used for this function in order to buffer the data
// being sent using the SPI0CN0_TXNF flag.
//
//==============================================================================

/*******************************************************************************
 * @fn void SPI_transfer_data(uint8_t[], uint8_t)
 *
 * @brief :: Send data to slave
 *
 * @param writeData :: Commands to send in Data array
 * @param Max_Buffer_Size :: Buffer size of the Data array
 *
 ******************************************************************************/

void SPI_transfer_data(uint8_t writeData[], uint8_t Max_Buffer_Size)
{
   uint8_t array_index;
   uint8_t SFRPAGE_save = SFRPAGE;

   while(!SPI0CN0_NSSMD0);             // Wait until the SPI is free, in case
                                       // it's already busy

   SFRPAGE = PG3_PAGE;

   IE_ESPI0 = 0;                       // Disable SPI interrupts

   SPI0CN0_NSSMD0 = 0;                 // Enable the slave

   while(!SPI0CN0_TXNF);               // Wait until the FIFO isn't full
   SPI0DAT = SPI_WRITE_BUFFER;         // Load the XMIT register

   for(array_index = 0; array_index < Max_Buffer_Size; array_index++)
   {
      while(!SPI0CN0_TXNF);               // Wait until the FIFO isn't full
      SPI0DAT = writeData[array_index]; // Load the data into the buffer
   }

   // Wait until TX FIFO count is 0
   // (the FIFO is empty, but the shift register may contain the last
   // byte being transmitted)
   while ((SPI0FCT & SPI0FCT_TXCNT__FMASK) >> SPI0FCT_TXCNT__SHIFT);

   // Wait until the last byte in the shift register is transferred
   while (SPI0CFG & SPI0CFG_SPIBSY__SET);

   SPI0CN0_SPIF = 0;

   // Flush the data received during the write transfer
   SPI0FCN0 |= SPI0FCN0_RFLSH__BMASK;
   while (SPI0FCN0 & SPI0FCN0_RFLSH__BMASK);

   SPI0CN0_NSSMD0 = 1;                 // Disable the Slave

   IE_ESPI0 = 1;                       // Re-enable SPI interrupts

   SFRPAGE = SFRPAGE_save;
}



/*******************************************************************************
 * @fn void SCAN_LIMIT_REGISTER()
 *
 * @brief ::  The scan-limit register sets how many digits are displayed,
              from 1 to 8. They are displayed in a multiplexed manner
              with a typical display scan rate of 800Hz with 8 digits
              displayed.
              the scan rate is 8fOSC/N, where N is the number of digits scanned

******************************************************************************/

void SCAN_LIMIT_REGISTER()
{
  uint8_t SEND_DATA[COMMAND_BUFFER_SIZE] = {SCAN_LIMIT_CMD,7};

  SPI_transfer_data(SEND_DATA,COMMAND_BUFFER_SIZE);
}



/*******************************************************************************
 * @fn void DECODE_MODE_REGISTER()
 *
 * @brief ::  The decode-mode register sets BCD code B (0-9, E, H, L,
              P, and -) or no-decode operation for each digit.
 *
******************************************************************************/

void DECODE_MODE_REGISTER()
{
  uint8_t SEND_DATA[COMMAND_BUFFER_SIZE] = {DECODE_CMD,DECODE_DATA_VAL};

  SPI_transfer_data(SEND_DATA,COMMAND_BUFFER_SIZE);
}



/*******************************************************************************
 * @fn void SHUTDOWN_REGISTER_MODE(uint8_t)
 *
 * @brief ::  When the MAX7219 is in shutdown mode, the scan oscillator is
              halted, all segment current sources are pulled to ground,
              and all digit drivers are pulled to V+, thereby blanking the
              display.
 *
 * @param SHUTOWN_MODE_SELECT :: start = 0x00
 *                               stop  = 0x01
******************************************************************************/

void SHUTDOWN_REGISTER_MODE(uint8_t SHUTOWN_MODE_SELECT)
{
  uint8_t SEND_DATA_START[COMMAND_BUFFER_SIZE] = {SHUTDOWN_CMD,start};
  uint8_t SEND_DATA_STOP[COMMAND_BUFFER_SIZE] = {SHUTDOWN_CMD,stop};

  switch(SHUTOWN_MODE_SELECT)
  {
    case 0x00 :

      SPI_transfer_data(SEND_DATA_START,COMMAND_BUFFER_SIZE);
      break;

    case 0x01 :

      SPI_transfer_data(SEND_DATA_STOP,COMMAND_BUFFER_SIZE);
      break;
  }
}



/*******************************************************************************
 * @fn void DISPLAY_TEST_MODE(uint8_t)
 *
 * @brief ::  Display-test mode turns all LEDs on by overriding,
              but not altering. In display-test mode, 8 digits
              are scanned and the duty cycle is 31/32 (15/16 for MAX7221).

 * @param DISPLAY_MODE_SELECT :: disp_stop  = 0,
 *                               disp_start = 1
 ******************************************************************************/

void DISPLAY_TEST_MODE(uint8_t DISPLAY_MODE_SELECT)
{
  uint8_t SEND_DATA_DISP_STOP[COMMAND_BUFFER_SIZE] = {DISPLAY_TEST_CMD,disp_stop};
  uint8_t SEND_DATA_DISP_START[COMMAND_BUFFER_SIZE] = {DISPLAY_TEST_CMD,disp_start};

  switch(DISPLAY_MODE_SELECT)
  {
    case 0x00 :

      SPI_transfer_data(SEND_DATA_DISP_STOP,COMMAND_BUFFER_SIZE);
      break;

    case 0x01 :

      SPI_transfer_data(SEND_DATA_DISP_START,COMMAND_BUFFER_SIZE);
      break;
  }
}

/*******************************************************************************
 * @fn void BRIGHTNESS_SETTING_MODE(uint8_t)
 *
 * @brief :: It controls the brightness of the segment Display.
 *
 * @param Brightness_Value :: Brightness_Value varies between 0-15 (b0-b15)
 *
 ******************************************************************************/

void BRIGHTNESS_SETTING_MODE(uint8_t Brightness_Value)  //   Brightness_Value varies between 0-15
{
  uint8_t INTENSITY_DATA[INTENSITY_BUFFER_SIZE] = {INTENSITY_CONTROL_CMD,b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14,b15};
  uint8_t INTENSITY_DATA_SEND[COMMAND_BUFFER_SIZE];

  INTENSITY_DATA_SEND[first_byte] = INTENSITY_DATA[0];
  INTENSITY_DATA_SEND[second_byte] = INTENSITY_DATA[Brightness_Value + 1];

  SPI_transfer_data(INTENSITY_DATA_SEND,COMMAND_BUFFER_SIZE);
}

/*******************************************************************************
 * @fn void CLEAR_DISPLAY()
 *
 * @brief :: Clear all segment display.
 *
 ******************************************************************************/

void CLEAR_DISPLAY()
{
  uint8_t clear_loop = 0;
  uint8_t CLEAR_DISP_POSITION[8] = {1,2,3,4,5,6,7,8};
  uint8_t CLEAR_DISP[2];


  for(clear_loop = 0; clear_loop < 8; clear_loop++)
    {
      CLEAR_DISP[first_byte] = CLEAR_DISP_POSITION[clear_loop];
      CLEAR_DISP[second_byte] = 0x00;
      SPI_transfer_data(CLEAR_DISP,COMMAND_BUFFER_SIZE);
    }

}


/*******************************************************************************
 * @fn void MAX7219_START()
 *
 * @brief ::  The initializing process to configure all MAX7219 IC settings
 *            to start segment display.
 ******************************************************************************/

void MAX7219_START()
{
  SCAN_LIMIT_REGISTER();
  DECODE_MODE_REGISTER();
  SHUTDOWN_REGISTER_MODE(stop);
  DISPLAY_TEST_MODE(disp_stop);
  CLEAR_DISPLAY();
  BRIGHTNESS_SETTING_MODE(b1);

}

/*******************************************************************************
 * @fn void setdigit(uint8_t, uint8_t)
 *
 * @brief ::  It is used to set digits (0-9) on Eight 7-segment.
 *
 * @param digit :: Put any digit 0-9 which you want to show on display.
 *
 * @param disp_loc ::  d1-d8 show position on which segment display
 *                     you want to show.
 *
 ******************************************************************************/

void setdigit(uint8_t digit,uint8_t disp_loc )
{
  uint8_t DIGIT_COMMAND[DIGIT_COMMAND_BUFFER]={zero,one,two,three,four,five,six,seven,eight,nine};
  uint8_t DISPLAY_LOC_COMMAND[DISPLAY_COMMAND_BUFFER] = {1,2,3,4,5,6,7,8};

  uint8_t SEND_DIGIT_CMD[COMMAND_BUFFER_SIZE] = {0};

  SEND_DIGIT_CMD[first_byte] = DISPLAY_LOC_COMMAND[disp_loc];
  SEND_DIGIT_CMD[second_byte] = DIGIT_COMMAND[digit];

  SPI_transfer_data(SEND_DIGIT_CMD,COMMAND_BUFFER_SIZE);

}

void setalphabet(uint8_t alphabet,uint8_t disp_loc)
{
  uint8_t DISPLAY_LOC_COMMAND[DISPLAY_COMMAND_BUFFER] = {1,2,3,4,5,6,7,8};

  uint8_t SEND_ALPHABET_CMD[COMMAND_BUFFER_SIZE] = {0};

  SEND_ALPHABET_CMD[first_byte] = DISPLAY_LOC_COMMAND[disp_loc];
  SEND_ALPHABET_CMD[second_byte] = alphabet;

  SPI_transfer_data(SEND_ALPHABET_CMD,COMMAND_BUFFER_SIZE);
}
/*******************************************************************************
 * @fn void DisplayTest()
 *
 * @brief :: It is used to display all digits on Eight 7-segment.
 *
 ******************************************************************************/
void DisplayTest()
{
  setdigit(0,d1);
  setdigit(1,d2);
  setdigit(2,d3);
  setdigit(3,d4);

  setdigit(4,d5);
  setdigit(5,d6);
  setdigit(6,d7);
  setdigit(7,d8);
  delay_ms(500);

  CLEAR_DISPLAY();
  delay_ms(500);

  setdigit(8,d1);
  setdigit(9,d2);

  setalphabet(A,d3);
  setalphabet(b,d4);
  setalphabet(C,d5);
  setalphabet(d,d6);
  setalphabet(E,d7);
  setalphabet(F,d8);
  delay_ms(500);

  CLEAR_DISPLAY();
  delay_ms(500);
}

