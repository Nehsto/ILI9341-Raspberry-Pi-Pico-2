#ifndef ILI9341_HW_H
#define ILI9341_HW_H

// --- SPI ---
#define ILI9341_SPI_PORT    spi0
#define ILI9341_DC_PIN      21
#define ILI9341_RST_PIN     20
#define ILI9341_MISO_PIN    16
#define ILI9341_CS_PIN      17
#define ILI9341_SCK_PIN     18
#define ILI9341_MOSI_PIN    19
#define ILI9341_LED_PIN     22

#define ILI9341_PWM_SLICE   PWM_GPIO_SLICE_NUM(ILI9341_LED_PIN)
#define ILI9341_PWM_CLKDIV  4.f
#define DELAYMS_HOLD_IMAGE  5000

#define DATA_BIT            1
#define COMMAND_BIT         0

#define MHz 1000000UL

#define CMD_SLEEP_MS        5
#define HW_RESET_SLEEP_MS   10      // Should be 5ms but just to be safe
#define BOOTUP_SLEEP_MS     150     // Should be 120ms but just to be safe

#define ILI9341_SWRESET     0x01
#define ILI9341_GAMMASET    0x26
#define ILI9341_GMCTRP1     0xE0    // Positive Gamma Correction:
#define ILI9341_GMCTRN1     0xE1    // Negative Gamma Correction:
#define ILI9341_MADCTL      0x36    // Memory Access Control:
#define ILI9341_PIXFMT      0x3A    // Pixel Format: 0x00 first, then 0x55 (16 bit) or 0x66 (18 bit)
#define ILI9341_FRMCTL1     0xB1    // Frame Rate Control: 
#define ILI9341_SLEEPOUT    0x11
#define ILI9341_IDLEOFF     0x38
#define ILI9341_DISPLAYOFF  0x28
#define ILI9341_DISPLAYON   0x29

#define ILI9341_RGBISC      0xB0    // RGB Interface Signal Control: RCM Bits set to '0b10' to write colors to display
#define ILI9341_CASET       0x2A    // Column Address Set
#define ILI9341_PASET       0x2B    // Page Address Set
#define ILI9341_RAMWR       0x2C    // Memory Write

#define ILI9341_WIDTH       320  
#define ILI9341_HEIGHT      240 

#include <pico/stdlib.h>
#include "hardware/spi.h"
#include "hardware/pwm.h"
#include "pico/binary_info.h"

#endif