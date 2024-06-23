/*
 * MAX7219_EFM8.h
 *
 *  Created on: 21-Jun-2024
 *      Author: visha
 */

#ifndef INC_MAX7219_EFM8_H_
#define INC_MAX7219_EFM8_H_


//=============================================================================
// User Includes
//=============================================================================

#include <SI_EFM8BB3_Register_Enums.h>                  // SFR declarations
#include "InitDevice.h"


//=============================================================================
// Global Constants
//=============================================================================


#define SYSCLK                24500000 // Internal oscillator frequency in Hz

#define SPI_CLOCK               500000 // Maximum SPI clock
// The SPI clock is a maximum of 1 MHz


#define MAX_BUFFER_SIZE            8 // Maximum buffer Master will send

// Instruction Set

#define  SPI_WRITE                 0x04 // Send a byte from the Master to the
// Slave
#define  SPI_READ                  0x08 // Send a byte from the Slave to the
// Master
#define  SPI_WRITE_BUFFER          0x10 // Send a series of bytes from the
// Master to the Slave
#define  SPI_READ_BUFFER           0x20 // Send a series of bytes from the Slave
// to the Master
#define  ERROR_OCCURRED            0x40 // Indicator for the Slave to tell the
// Master an error occurred
#define COMMAND_BUFFER_SIZE        2

#define INTENSITY_BUFFER_SIZE      17

#define DIGIT_COMMAND_BUFFER       10

#define DISPLAY_COMMAND_BUFFER     8

//MAX7219

#define INTENSITY_CONTROL_CMD     0x0A
#define SCAN_LIMIT_CMD            0x0B
#define SHUTDOWN_CMD              0x0C
#define DISPLAY_TEST_CMD          0x0F


#define DECODE_CMD                0x09
#define DECODE_DATA_VAL           0x00         // No decode mode register is select


///=============================================================================
/// SPI communication Functions ::
///=============================================================================

void SPI_transfer_data(uint8_t writeData[], uint8_t size);

///=============================================================================
/// Delay Functions ::
///=============================================================================

void delay_us (uint16_t counts);
uint16_t delay_ms (uint16_t ms_counts);

///=============================================================================
/// MAX7219 Functions ::
///=============================================================================

void MAX7219_START();
void CLEAR_DISPLAY();
void SCAN_LIMIT_REGISTER();
void DECODE_MODE_REGISTER();
void SHUTDOWN_REGISTER_MODE(uint8_t SHUTOWN_MODE_SELECT);
void DISPLAY_TEST_MODE(uint8_t DISPLAY_MODE_SELECT);
void BRIGHTNESS_SETTING_MODE(uint8_t Brightness_Value);  //   Brightness_Value varies between 0-15

///=============================================================================
/// User Functions ::
///=============================================================================

void DisplayTest();
void setdigit(uint8_t digit_commands ,uint8_t digit_locations);


//-----------------------------------------------------------------------------
// Enumerations
//-----------------------------------------------------------------------------

enum BYTE_ORDER
{
  first_byte,
  second_byte,
};
enum STATE
{
  LOW,
  HIGH,
};

typedef enum PORT_PINS
{
  kdata,
  kclock,
  kchipSelect,
};

typedef enum BIT_SEQUENCES
{
  LSBFIRST ,
  MSBFIRST,
};


typedef enum DISPLAY_MODE_SELECT
{
  disp_stop   = 0x00,
  disp_start  = 0x01,

};

typedef enum SHUTOWN_MODE_SELECT
{
  start = 0x00,
  stop   = 0x01
};

enum BRIGHTNESS_SETTING_SELECT
{
  b0   = 0x00,
  b1   = 0x01,
  b2   = 0x02,
  b3   = 0x03,
  b4   = 0x04,
  b5   = 0x05,
  b6   = 0x06,
  b7   = 0x07,
  b8   = 0x08,
  b9   = 0x09,
  b10  = 0x0A,
  b11  = 0x0B,
  b12  = 0x0C,
  b13  = 0x0D,
  b14  = 0x0E,
  b15  = 0x0F,
};

typedef enum MAX7219_DIGITS_COMMANDS
{
  zero  = 0x7e,
  one   = 0x30,     //0x06
  two   = 0x6d,
  three = 0x79,
  four  = 0x33,
  five  = 0x5b,
  six   = 0x5f,
  seven = 0x70,
  eight = 0x7f,
  nine  = 0x7b,
  dp    = 0x80,
  blank_blk   = 0x08,
  blank_dgt   = 0x00,
  blank_2_blk = 0x48,
  minus_sign  = 0x40,
}digit_commands;

typedef enum DIGIT_LOCATIONS
{
  d8  ,
  d7  ,
  d6  ,
  d5  ,
  d4  ,
  d3  ,
  d2  ,
  d1  ,

}digit_locations;

#endif /* INC_MAX7219_EFM8_H_ */
