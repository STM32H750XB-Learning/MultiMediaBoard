/**
  ******************************************************************************
  * @file    ILI9488.h
  * @author  MXH
  * @note    Using SPI Mode 0, MSB, data width 8
  *
  ******************************************************************************
  */
#ifndef ILI9488_H__
#define ILI9488_H__

#include "main.h"
#include "spi.h"
#include "app_threadx.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Private define ------------------------------------------------------------*/
/* ILI9488 Command */
/* Standard Command */
#define ILI9488_CMD_NOP 						(0x00U)
#define ILI9488_CMD_SOFT_RESET 				 	(0x01U)
#define ILI9488_CMD_READ_DISPLAY_ID_INFO		(0x04U)	// Read display identification information
#define ILI9488_CMD_READ_NUM_OF_ERROR_ON_DSI 	(0x05U)	// Read Number of the Errors on DSI
#define ILI9488_CMD_READ_DISPLAY_STATUS		 	(0x09U)
#define ILI9488_CMD_READ_DISPLAY_POWER_MODE	 	(0x0AU)
#define ILI9488_CMD_READ_DISPLAY_MADCTL		 	(0x0BU)
#define ILI9488_CMD_READ_PIXEL_FORMAT		 	(0x0CU)
#define ILI9488_CMD_READ_DISPLAY_IMAGE_MODE		(0x0DU)
#define ILI9488_CMD_READ_DISPLAY_SIGNAL_MODE	(0x0EU)
#define ILI9488_CMD_READ_DISPLAY_SELF_DIAGNOSTIC_RESULT	(0x0FU)

#define ILI9488_CMD_SLEEP_IN					(0x10U)
#define ILI9488_CMD_SLEEP_OUT					(0x11U)
#define ILI9488_CMD_PARTIAL_MODE_ON				(0x12U)
#define ILI9488_CMD_NORMAL_DISPLAY_MODE_ON		(0x13U)

#define ILI9488_CMD_DISPLAY_INVERSION_OFF		(0x20U)
#define ILI9488_CMD_DISPLAY_INVERSION_ON		(0x21U)
#define ILI9488_CMD_ALL_PIXEL_OFF				(0x22U)
#define ILI9488_CMD_ALL_PIXEL_ON				(0x23U)
#define ILI9488_CMD_DISPLAY_OFF					(0x28U)
#define ILI9488_CMD_DISPLAY_ON					(0x29U)
#define ILI9488_CMD_COLUMN_ADDR_SET				(0x2AU)
#define ILI9488_CMD_PAGE_ADDR_SET				(0x2BU)
#define ILI9488_CMD_MEMORY_WRITE				(0x2CU)
#define ILI9488_CMD_MEMORY_READ					(0x2EU)

#define ILI9488_CMD_PARTIAL_AREA				(0x30U)
#define ILI9488_CMD_VERTICAL_SCROLLING_DEF		(0x33U)	// Vertical Scrolling Definition
#define ILI9488_CMD_TEARING_EFFECT_LINE_OFF		(0x34U)
#define ILI9488_CMD_TEARING_EFFECT_LINE_ON		(0x35U)
#define ILI9488_CMD_MEMORY_ACCESS_CONTROL		(0x36U)
#define ILI9488_CMD_VERTICAL_SCROLLING_START_ADDR	(0x37U)
#define ILI9488_CMD_IDLE_MODE_OFF				(0x38U)
#define ILI9488_CMD_IDLE_MODE_ON				(0x39U)
#define ILI9488_CMD_INTERFACE_PIXEL_FORMAT		(0x3AU)
#define ILI9488_CMD_MEMORY_WRITE_CONTINUE		(0x3CU)
#define ILI9488_CMD_MEMORY_READ_CONTINUE		(0x3EU)

#define ILI9488_CMD_WRITE_TEAR_SCAN_LINE		(0x44U)
#define ILI9488_CMD_READ_TEAR_SCAN_LINE			(0x45U)

#define ILI9488_CMD_WRITE_DISPLAY_BRIGHTNESS_VALUE					(0x51U)
#define ILI9488_CMD_READ_DISPLAY_BRIGHTNESS_VALUE					(0x52U)
#define ILI9488_CMD_WRITE_CTRL_DISPLAY_VAULE						(0x53U)
#define ILI9488_CMD_READ_CTRL_DISPLAY_VAULE							(0x54U)
#define ILI9488_CMD_WRITE_CONTENT_ADAPTIVE_BRIGHTNESS_CONTROL_VALUE	(0x55U)
#define ILI9488_CMD_READ_CONTENT_ADAPTIVE_BRIGHTNESS_CONTROL_VALUE	(0x56U)
#define ILI9488_CMD_WRITE_CABC_MINIMUM_BRIGHTNESS					(0x5EU)
#define ILI9488_CMD_READ_CABC_MINIMUM_BRIGHTNESS					(0x5FU)

#define ILI9488_CMD_READ_AUTOMATIC_BRIGHTNESS	(0x68U)	// Read automatic brightness, control self-diagnostic result
#define ILI9488_CMD_READ_ID1	(0xDAU)
#define ILI9488_CMD_READ_ID2	(0xDBU)
#define ILI9488_CMD_READ_ID3	(0xDCU)

/* Extended Command */
#define ILI9488_CMD_INTERFACE_MODE_CONTROL		(0xB0U)
#define ILI9488_CMD_FRAME_RATE_CONTROL_NORMAL	(0xB1U)	// Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9488_CMD_FRAME_RATE_CONTROL_IDLE		(0xB2U)	// Frame Rate Control (In Idle Mode/8 colors)
#define ILI9488_CMD_FRAME_RATE_CONTROL_PARTIAL	(0xB3U)	// Frame Rate Control (In Partial Mode/Full colors)
#define ILI9488_CMD_DISPLAY_INVERSION_CONTROL	(0xB4U)
#define ILI9488_CMD_BLANKING_PORCH_CONTROL		(0xB5U)
#define ILI9488_CMD_DISPLAY_FUNCTION_CONTROL	(0xB6U)
#define ILI9488_CMD_ENTRY_MODE_SET				(0xB7U)
#define ILI9488_CMD_COLOR_ENHANCEMENT_CONTROL1	(0xB9U)
#define ILI9488_CMD_COLOR_ENHANCEMENT_CONTROL2	(0xBAU)	
#define ILI9488_CMD_HS_LANES_CONTROL			(0xBEU)

#define ILI9488_CMD_POWER_CONTROL1				(0xC0U)
#define ILI9488_CMD_POWER_CONTROL2				(0xC1U)
#define ILI9488_CMD_POWER_CONTROL3				(0xC2U)
#define ILI9488_CMD_POWER_CONTROL4				(0xC3U)
#define ILI9488_CMD_POWER_CONTROL5				(0xC4U)
#define ILI9488_CMD_VCOM_CONTROL1				(0xC5U)
#define ILI9488_CMD_CABC_CONTROL1				(0xC6U)
#define ILI9488_CMD_CABC_CONTROL2				(0xC8U)
#define ILI9488_CMD_CABC_CONTROL3				(0xC9U)
#define ILI9488_CMD_CABC_CONTROL4				(0xCAU)
#define ILI9488_CMD_CABC_CONTROL5				(0xCBU)
#define ILI9488_CMD_CABC_CONTROL6				(0xCCU)
#define ILI9488_CMD_CABC_CONTROL7				(0xCDU)
#define ILI9488_CMD_CABC_CONTROL8				(0xCEU)
#define ILI9488_CMD_CABC_CONTROL9				(0xCFU)

#define ILI9488_CMD_NV_MEMORY_WRITE				(0xD0U)
#define ILI9488_CMD_NV_MEMORY_PROTECTION_KEY	(0xD1U)
#define ILI9488_CMD_NV_MEMORY_STATUS_READ		(0xD2U)
#define ILI9488_CMD_READ_ID4					(0xD3U)
#define ILI9488_CMD_ADJUST_CONTROL1				(0xD7U)
#define ILI9488_CMD_READ_ID4_CHECK				(0xD8U)

#define ILI9488_CMD_POSITIVE_GAMMA_CONTROL		(0xE0U)
#define ILI9488_CMD_NEGATIVE_GAMMA_CONTROL		(0xE1U)
#define ILI9488_CMD_DIGITAL_GAMMA_CONTROL1		(0xE2U)
#define ILI9488_CMD_DIGITAL_GAMMA_CONTROL2		(0xE3U)
#define ILI9488_CMD_SET_IMAGE_FUNCTION			(0xE9U)

#define ILI9488_CMD_ADJUST_CONTROL2				(0xF2U)
#define ILI9488_CMD_ADJUST_CONTROL3				(0xF7U)
#define ILI9488_CMD_ADJUST_CONTROL4				(0xF8U)
#define ILI9488_CMD_ADJUST_CONTROL5				(0xF9U)
#define ILI9488_CMD_SPI_READ_COMMAND_SETTING	(0xFBU)
#define ILI9488_CMD_ADJUST_CONTROL6				(0xFCU)
#define ILI9488_CMD_ADJUST_CONTROL7				(0xFFU)

/* POINT_COLOR */
#define ILI9488_POINT_COLOR_WHITE 			(0xFFFFFFUL)
#define ILI9488_POINT_COLOR_BLACK           (0x000000UL)
#define ILI9488_POINT_COLOR_BLUE            (0x0000FFUL)
#define ILI9488_POINT_COLOR_BRED            (0x00F81FUL)
#define ILI9488_POINT_COLOR_GRED            (0x00FFE0UL)
#define ILI9488_POINT_COLOR_GBLUE           (0x0007FFUL)
#define ILI9488_POINT_COLOR_RED             (0xFF0000UL)
#define ILI9488_POINT_COLOR_MAGENTA         (0x00F81FUL)
#define ILI9488_POINT_COLOR_GREEN           (0x00FF00UL)
#define ILI9488_POINT_COLOR_CYAN            (0x007FFFUL)
#define ILI9488_POINT_COLOR_YELLOW          (0x00FFE0UL)
#define ILI9488_POINT_COLOR_BROWN           (0x00BC40UL)
#define ILI9488_POINT_COLOR_BRRED           (0x00FC07UL)
#define ILI9488_POINT_COLOR_GRAY            (0x008430UL)
#define ILI9488_POINT_COLOR_GRAY175         (0x00AD75UL)
#define ILI9488_POINT_COLOR_GRAY151         (0x0094B2UL)
#define ILI9488_POINT_COLOR_GRAY187         (0x00BDD7UL)
#define ILI9488_POINT_COLOR_GRAY240       	(0x00F79EUL)

void ili9488_init(void);
void ili9488_enter_sleep(void);
void ili9488_exit_sleep(void);
void ili9488_set_color(uint32_t back, uint32_t fore);
void ili9488_set_address(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

void ili9488_draw_point_color(uint16_t x, uint16_t y, uint32_t color);
void ili9488_draw_point(uint16_t x, uint16_t y);
void ili9488_fill_whole(uint32_t color);
void ili9488_fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint32_t color);
void ili9488_fill_with_color_array(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint32_t *buf);
void ili9488_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ili9488_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ili9488_draw_circle(uint16_t x0, uint16_t y0, uint16_t r);
void ili9488_show_image(uint16_t x, uint16_t y, uint16_t length, uint16_t wide, const uint8_t *p);

#ifdef __cplusplus
extern }
#endif
#endif