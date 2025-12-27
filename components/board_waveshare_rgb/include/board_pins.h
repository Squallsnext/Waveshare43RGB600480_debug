/**
 * @file board_pins.h
 * @brief GPIO Pin definitions for Waveshare ESP32-S3-Touch-LCD-4.3
 *
 * TODO: VERIFY pins against Waveshare demo / schematic before production use.
 *       Source: docs/ESP32-S3-Touch-LCD-4.3-Sch.pdf
 *       Some pins (R3, R4) not clearly documented in schematic table.
 */

#pragma once

#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * LCD Panel Configuration
 ******************************************************************************/

/* Display Resolution - Schematic says "800x480 Pixels" */
#define LCD_H_RES               800
#define LCD_V_RES               480

/* Pixel Clock and Timing
 * Conservative 16MHz for stability (can increase after VSync smoke test) */
#define LCD_PCLK_HZ             (16 * 1000 * 1000)

/* Timing parameters (typical for 800x480 RGB panels) */
#define LCD_HSYNC_PULSE_WIDTH   4
#define LCD_HSYNC_BACK_PORCH    8
#define LCD_HSYNC_FRONT_PORCH   8
#define LCD_VSYNC_PULSE_WIDTH   4
#define LCD_VSYNC_BACK_PORCH    8
#define LCD_VSYNC_FRONT_PORCH   8

/*******************************************************************************
 * LCD Control Pins (from Schematic Table)
 ******************************************************************************/

#define LCD_GPIO_PCLK           GPIO_NUM_7
#define LCD_GPIO_HSYNC          GPIO_NUM_46
#define LCD_GPIO_VSYNC          GPIO_NUM_3
#define LCD_GPIO_DE             GPIO_NUM_5   /* TODO: VERIFY - Schematic shows both IO5 and IO6 for DE */

/*******************************************************************************
 * LCD RGB Data Pins - RGB565 (16-bit)
 *
 * From Schematic Table "WROOM-1/2 LCD":
 *   Blue:  B3=IO14, B4=IO38, B5=IO1, B6=IO0, B7=IO15
 *   Green: G2=IO39, G3=IO2, G4=IO45, G5=IO48, G6=IO47, G7=IO21
 *   Red:   R5=IO42, R6=IO41, R7=IO40
 *
 * WARNING: R3 and R4 NOT FOUND in schematic pinout table!
 *          Using IO4 and IO6 as placeholders based on adjacent GPIO numbers.
 *          TODO: VERIFY R3, R4 against Waveshare demo code or hardware test.
 ******************************************************************************/

/* Blue channel (5 bits: B3-B7, B0-B2 unused for RGB565) */
#define LCD_GPIO_B3             GPIO_NUM_14
#define LCD_GPIO_B4             GPIO_NUM_38
#define LCD_GPIO_B5             GPIO_NUM_1
#define LCD_GPIO_B6             GPIO_NUM_0
#define LCD_GPIO_B7             GPIO_NUM_15

/* Green channel (6 bits: G2-G7, G0-G1 unused for RGB565) */
#define LCD_GPIO_G2             GPIO_NUM_39
#define LCD_GPIO_G3             GPIO_NUM_2
#define LCD_GPIO_G4             GPIO_NUM_45
#define LCD_GPIO_G5             GPIO_NUM_48
#define LCD_GPIO_G6             GPIO_NUM_47
#define LCD_GPIO_G7             GPIO_NUM_21

/* Red channel (5 bits: R3-R7, R0-R2 unused for RGB565) */
/* TODO: VERIFY R3, R4 - these GPIOs are NOT in the schematic table! */
#define LCD_GPIO_R3             GPIO_NUM_4   /* PLACEHOLDER - NOT VERIFIED */
#define LCD_GPIO_R4             GPIO_NUM_6   /* PLACEHOLDER - NOT VERIFIED (conflicts with DE?) */
#define LCD_GPIO_R5             GPIO_NUM_42
#define LCD_GPIO_R6             GPIO_NUM_41
#define LCD_GPIO_R7             GPIO_NUM_40

/* Convenience macro: All 16 data GPIOs in order for esp_lcd_rgb_panel_config_t
 * Order: B[4:0], G[5:0], R[4:0] (LSB to MSB within each channel) */
#define LCD_DATA_GPIO_NUMS      \
    LCD_GPIO_B3, LCD_GPIO_B4, LCD_GPIO_B5, LCD_GPIO_B6, LCD_GPIO_B7, \
    LCD_GPIO_G2, LCD_GPIO_G3, LCD_GPIO_G4, LCD_GPIO_G5, LCD_GPIO_G6, LCD_GPIO_G7, \
    LCD_GPIO_R3, LCD_GPIO_R4, LCD_GPIO_R5, LCD_GPIO_R6, LCD_GPIO_R7

/*******************************************************************************
 * LCD Backlight & Reset (via CH422G I2C IO Expander)
 *
 * CH422G address: 0x24 (7-bit)
 * EXIO1 = CTP_RST (Touch Reset)
 * EXIO3 = DISP (Backlight Enable)
 * EXIO4 = LCD_RST (LCD Reset)
 ******************************************************************************/

#define LCD_GPIO_BL             GPIO_NUM_NC  /* Via CH422G EXIO3, not direct GPIO */
#define LCD_GPIO_RST            GPIO_NUM_NC  /* Via CH422G EXIO4, not direct GPIO */

/* CH422G IO Expander (if direct control needed) */
#define IO_EXPANDER_I2C_ADDR    0x24
#define IO_EXPANDER_EXIO_DISP   3   /* Backlight enable bit */
#define IO_EXPANDER_EXIO_RST    4   /* LCD reset bit */

/*******************************************************************************
 * Touch Panel (GT911 Capacitive Touch Controller)
 ******************************************************************************/

#define TOUCH_I2C_SDA           GPIO_NUM_8
#define TOUCH_I2C_SCL           GPIO_NUM_9
#define TOUCH_GPIO_INT          GPIO_NUM_NC  /* TODO: Schematic shows IO5 but conflicts with DE */
#define TOUCH_GPIO_RST          GPIO_NUM_NC  /* Via CH422G EXIO1, not direct GPIO */

/* GT911 I2C address (depends on RST/INT timing during power-up) */
#define TOUCH_I2C_ADDR          0x5D  /* or 0x14 depending on init sequence */

/*******************************************************************************
 * SD Card (optional, documented for completeness)
 ******************************************************************************/

#define SD_GPIO_MOSI            GPIO_NUM_10
#define SD_GPIO_MISO            GPIO_NUM_11
#define SD_GPIO_CLK             GPIO_NUM_12
#define SD_GPIO_CS              GPIO_NUM_13

#ifdef __cplusplus
}
#endif
