/**
 * @file board_pins.h
 * @brief GPIO Pin definitions for Waveshare ESP32-S3-Touch-LCD-4.3
 *
 * Source: Waveshare 08_lvgl_Porting demo (waveshare_rgb_lcd_port.h)
 * Verified: 2025-12-27
 */

#pragma once

#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * LCD Panel Configuration (Verified from Waveshare Demo)
 ******************************************************************************/

#define LCD_H_RES               800
#define LCD_V_RES               480
#define LCD_BITS_PER_PIXEL      16   /* RGB565 */

/*******************************************************************************
 * LCD Timing (from waveshare_rgb_lcd_port.c)
 ******************************************************************************/

#define LCD_PCLK_HZ             (16 * 1000 * 1000)  /* 16 MHz */
#define LCD_HSYNC_PULSE_WIDTH   4
#define LCD_HSYNC_BACK_PORCH    8
#define LCD_HSYNC_FRONT_PORCH   8
#define LCD_VSYNC_PULSE_WIDTH   4
#define LCD_VSYNC_BACK_PORCH    8
#define LCD_VSYNC_FRONT_PORCH   8
#define LCD_PCLK_ACTIVE_NEG     1    /* Important: Active low pixel clock */

/*******************************************************************************
 * LCD Control Pins (from waveshare_rgb_lcd_port.h lines 39-42)
 ******************************************************************************/

#define LCD_GPIO_VSYNC          GPIO_NUM_3
#define LCD_GPIO_HSYNC          GPIO_NUM_46
#define LCD_GPIO_DE             GPIO_NUM_5
#define LCD_GPIO_PCLK           GPIO_NUM_7
#define LCD_GPIO_DISP           GPIO_NUM_NC  /* -1, via CH422G IO Expander */
#define LCD_GPIO_RST            GPIO_NUM_NC  /* -1, via CH422G IO Expander */

/*******************************************************************************
 * LCD RGB Data Pins - RGB565 16-bit (from waveshare_rgb_lcd_port.h lines 43-58)
 *
 * Order: DATA0-DATA15 maps to hardware RGB565 bus
 ******************************************************************************/

#define LCD_GPIO_DATA0          GPIO_NUM_14
#define LCD_GPIO_DATA1          GPIO_NUM_38
#define LCD_GPIO_DATA2          GPIO_NUM_18
#define LCD_GPIO_DATA3          GPIO_NUM_17
#define LCD_GPIO_DATA4          GPIO_NUM_10
#define LCD_GPIO_DATA5          GPIO_NUM_39
#define LCD_GPIO_DATA6          GPIO_NUM_0
#define LCD_GPIO_DATA7          GPIO_NUM_45
#define LCD_GPIO_DATA8          GPIO_NUM_48
#define LCD_GPIO_DATA9          GPIO_NUM_47
#define LCD_GPIO_DATA10         GPIO_NUM_21
#define LCD_GPIO_DATA11         GPIO_NUM_1
#define LCD_GPIO_DATA12         GPIO_NUM_2
#define LCD_GPIO_DATA13         GPIO_NUM_42
#define LCD_GPIO_DATA14         GPIO_NUM_41
#define LCD_GPIO_DATA15         GPIO_NUM_40

/* Convenience macro for esp_lcd_rgb_panel_config_t.data_gpio_nums */
#define LCD_DATA_GPIO_NUMS      \
    LCD_GPIO_DATA0, LCD_GPIO_DATA1, LCD_GPIO_DATA2, LCD_GPIO_DATA3,   \
    LCD_GPIO_DATA4, LCD_GPIO_DATA5, LCD_GPIO_DATA6, LCD_GPIO_DATA7,   \
    LCD_GPIO_DATA8, LCD_GPIO_DATA9, LCD_GPIO_DATA10, LCD_GPIO_DATA11, \
    LCD_GPIO_DATA12, LCD_GPIO_DATA13, LCD_GPIO_DATA14, LCD_GPIO_DATA15

/*******************************************************************************
 * Backlight Control via CH422G I2C IO Expander
 *
 * From waveshare_rgb_lcd_port.c:
 *   - CH422G config address: 0x24
 *   - CH422G data address: 0x38
 *   - Backlight ON:  write 0x1E to 0x38
 *   - Backlight OFF: write 0x1A to 0x38
 ******************************************************************************/

#define CH422G_ADDR_CONFIG      0x24
#define CH422G_ADDR_DATA        0x38
#define CH422G_OUTPUT_MODE      0x01
#define CH422G_BL_ON            0x1E
#define CH422G_BL_OFF           0x1A
#define CH422G_TOUCH_RESET      0x2C  /* For touch reset sequence */
#define CH422G_TOUCH_UNRESET    0x2E

/*******************************************************************************
 * Touch Panel (GT911 via I2C)
 *
 * From waveshare_rgb_lcd_port.h lines 18-19, 65-66
 ******************************************************************************/

#define TOUCH_I2C_NUM           I2C_NUM_0
#define TOUCH_I2C_SDA           GPIO_NUM_8
#define TOUCH_I2C_SCL           GPIO_NUM_9
#define TOUCH_I2C_FREQ_HZ       400000
#define TOUCH_GPIO_INT          GPIO_NUM_NC  /* -1, polling mode */
#define TOUCH_GPIO_RST          GPIO_NUM_NC  /* -1, via CH422G */
#define TOUCH_I2C_ADDR          0x5D         /* GT911 default address */

/*******************************************************************************
 * SD Card (optional, from schematic)
 ******************************************************************************/

#define SD_GPIO_MOSI            GPIO_NUM_10
#define SD_GPIO_MISO            GPIO_NUM_11
#define SD_GPIO_CLK             GPIO_NUM_12
#define SD_GPIO_CS              GPIO_NUM_13

#ifdef __cplusplus
}
#endif
