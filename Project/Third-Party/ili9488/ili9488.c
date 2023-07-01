/* Includes ------------------------------------------------------------------*/
#include "ili9488.h"
#include <stdio.h>
#include "app_azure_rtos.h"
#include "spi.h"

/* Private define ------------------------------------------------------------*/
#define ILI9488_WIDTH				(480U)
#define ILI9488_HEIGHT				(320U)
#define ILI9488_BITS_PER_PIXEL		(18U)	// RGB666
#define ILI9488_BUF_SIZE			(ILI9488_WIDTH * ILI9488_HEIGHT * 3)

#define ili9488_delay(ms)			tx_thread_sleep(ms)
#define ili9488_display_on()		ILI9488_PIN_BL_HIGH()
#define ili9488_display_off()		ILI9488_PIN_BL_LOW()

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern TX_SEMAPHORE tx_app_semaphore;
static uint32_t BACK_COLOR = ILI9488_POINT_COLOR_WHITE, FORE_COLOR = ILI9488_POINT_COLOR_BLACK;
__attribute__((section(".RAM_SDRAM"))) static uint8_t ili9488_buf[ILI9488_BUF_SIZE] = {0};
/* Private user code ---------------------------------------------------------*/
static uint32_t ili9488_send_data_dma(const void *data, size_t length)
{
    size_t remain_length, already_send_length;
    uint16_t send_length;
    const uint8_t *send_buf;
    HAL_StatusTypeDef state;

    ILI9488_PIN_DC_HIGH();
    remain_length = length;
    while(remain_length)
    {
        /* the HAL library use uint16_t to save the data length */
        if(remain_length > 0xFFFF)
        {
            send_length = 0xFFFF;
            remain_length -= send_length;
        }
        else
        {
            send_length = remain_length;
            remain_length = 0;
        }

        already_send_length = length - send_length - remain_length;
        send_buf = (uint8_t *)data + already_send_length;

        state = HAL_SPI_Transmit_DMA(&hspi2, (uint8_t *)data, send_length);
        tx_semaphore_get(&tx_app_semaphore, TX_WAIT_FOREVER);
        if(state != HAL_OK)	return already_send_length;
    }

    return length;
}

static HAL_StatusTypeDef ili9488_send_cmd(uint8_t cmd)
{
    ILI9488_PIN_DC_LOW();
    return HAL_SPI_Transmit(&hspi2, &cmd, 1, 1000);
}

static HAL_StatusTypeDef ili9488_send_bytes(uint8_t *buf, uint16_t size)
{
    ILI9488_PIN_DC_HIGH();
    return HAL_SPI_Transmit(&hspi2, buf, size, 1000);
}

static HAL_StatusTypeDef ili9488_send_byte(uint8_t byte)
{
    ILI9488_PIN_DC_HIGH();
    return HAL_SPI_Transmit(&hspi2, &byte, 1, 1000);
}

static HAL_StatusTypeDef ili9488_send_three_bytes(uint32_t three_bytes)
{
    uint8_t data[3] = {0};

    data[0] = (uint8_t)(three_bytes >> 16);
    data[1] = (uint8_t)(three_bytes >> 8);
    data[2] = (uint8_t)(three_bytes);

    ILI9488_PIN_DC_HIGH();
    return HAL_SPI_Transmit(&hspi2, data, 3, 1000);
}

static HAL_StatusTypeDef ili9488_read_byets(uint8_t *buf, size_t size)
{
    ILI9488_PIN_DC_HIGH();

    return HAL_SPI_Receive(&hspi2, buf, size, 1000);
}

/* Exported functions ---------------------------------------------------------*/
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi == &hspi2)	tx_semaphore_put(&tx_app_semaphore);
}

void ili9488_init(void)
{
    ili9488_display_off();
    ILI9488_PIN_RESET_HIGH();
    ili9488_delay(10);
    ILI9488_PIN_RESET_LOW();
    ili9488_delay(50);
    ILI9488_PIN_RESET_HIGH();
    ili9488_delay(200);

    ili9488_send_cmd(ILI9488_CMD_POSITIVE_GAMMA_CONTROL);
    ili9488_send_byte(0x00);
    ili9488_send_byte(0x07);
    ili9488_send_byte(0x0F);
    ili9488_send_byte(0x0D);
    ili9488_send_byte(0x1B);
    ili9488_send_byte(0x0A);
    ili9488_send_byte(0x3C);
    ili9488_send_byte(0x78);
    ili9488_send_byte(0x4A);
    ili9488_send_byte(0x07);
    ili9488_send_byte(0x0E);
    ili9488_send_byte(0x09);
    ili9488_send_byte(0x1B);
    ili9488_send_byte(0x1e);
    ili9488_send_byte(0x1F);

    ili9488_send_cmd(ILI9488_CMD_NEGATIVE_GAMMA_CONTROL);
    ili9488_send_byte(0x00);
    ili9488_send_byte(0x22);
    ili9488_send_byte(0x24);
    ili9488_send_byte(0x06);
    ili9488_send_byte(0x12);
    ili9488_send_byte(0x07);
    ili9488_send_byte(0x36);
    ili9488_send_byte(0x47);
    ili9488_send_byte(0x47);
    ili9488_send_byte(0x06);
    ili9488_send_byte(0x0a);
    ili9488_send_byte(0x07);
    ili9488_send_byte(0x30);
    ili9488_send_byte(0x37);
    ili9488_send_byte(0x0f);

    ili9488_send_cmd(ILI9488_CMD_POWER_CONTROL1);
    ili9488_send_byte(0x10);
    ili9488_send_byte(0x10);

    ili9488_send_cmd(ILI9488_CMD_POWER_CONTROL2);
    ili9488_send_byte(0x41);

    ili9488_send_cmd(ILI9488_CMD_VCOM_CONTROL1);
    ili9488_send_byte(0x00);
    ili9488_send_byte(0x22);
    ili9488_send_byte(0x80);

    ili9488_send_cmd(ILI9488_CMD_MEMORY_ACCESS_CONTROL);
    ili9488_send_byte(0x48);

    ili9488_send_cmd(ILI9488_CMD_INTERFACE_PIXEL_FORMAT);
    ili9488_send_byte(0x66);

    ili9488_send_cmd(ILI9488_CMD_INTERFACE_MODE_CONTROL);
    ili9488_send_byte(0x00);

    ili9488_send_cmd(ILI9488_CMD_FRAME_RATE_CONTROL_NORMAL);
    ili9488_send_byte(0xB0);
    ili9488_send_byte(0x11);

    ili9488_send_cmd(ILI9488_CMD_DISPLAY_INVERSION_CONTROL);
    ili9488_send_byte(0x02);

    ili9488_send_cmd(ILI9488_CMD_DISPLAY_FUNCTION_CONTROL);
    ili9488_send_byte(0x02);
    ili9488_send_byte(0x02);

    ili9488_send_cmd(ILI9488_CMD_ENTRY_MODE_SET);
    ili9488_send_byte(0xC6);

    ili9488_send_cmd(ILI9488_CMD_SET_IMAGE_FUNCTION);
    ili9488_send_byte(0x00);

    ili9488_send_cmd(ILI9488_CMD_ADJUST_CONTROL3);
    ili9488_send_byte(0xA9);
    ili9488_send_byte(0x51);
    ili9488_send_byte(0x2C);
    ili9488_send_byte(0x82);

    ili9488_send_cmd(ILI9488_CMD_SLEEP_OUT);
    ili9488_delay(120);
    ili9488_send_cmd(ILI9488_CMD_DISPLAY_ON);
    ili9488_delay(50);
}

void ili9488_enter_sleep(void)
{
    ili9488_display_off();
    ili9488_delay(5);
    ili9488_send_cmd(ILI9488_CMD_DISPLAY_OFF);
    ili9488_delay(10);
    ili9488_send_cmd(ILI9488_CMD_SLEEP_IN);
    ili9488_delay(120);
}

void ili9488_exit_sleep(void)
{
    ili9488_display_on();
    ili9488_delay(5);
    ili9488_send_cmd(ILI9488_CMD_SLEEP_OUT);
    ili9488_delay(120);
    ili9488_send_cmd(ILI9488_CMD_DISPLAY_ON);
    ili9488_delay(10);
}

void ili9488_set_color(uint32_t back, uint32_t fore)
{
    BACK_COLOR = back;
    FORE_COLOR = fore;
}

void ili9488_set_address(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    ili9488_send_cmd(ILI9488_CMD_COLUMN_ADDR_SET);
    ili9488_send_byte(x1 >> 8);
    ili9488_send_byte(x1);
    ili9488_send_byte(x2 >> 8);
    ili9488_send_byte(x2);

    ili9488_send_cmd(ILI9488_CMD_PAGE_ADDR_SET);
    ili9488_send_byte(y1 >> 8);
    ili9488_send_byte(y1);
    ili9488_send_byte(y2 >> 8);
    ili9488_send_byte(y2);

    ili9488_send_cmd(ILI9488_CMD_MEMORY_WRITE);
}

void ili9488_draw_point_color(uint16_t x, uint16_t y, uint32_t color)
{
    ili9488_set_address(x, y, x, y);
    ili9488_send_three_bytes(color);
}

void ili9488_draw_point(uint16_t x, uint16_t y)
{
    ili9488_draw_point_color(x, y, FORE_COLOR);
}

void ili9488_fill_whole(uint32_t color)
{
    uint32_t i, j;
    uint8_t data[3] = {0};
    uint8_t *buf = NULL;

//    data[0] = color >> 16;
//    data[1] = color >> 8;
//    data[2] = color;
    ili9488_set_address(0, 0, ILI9488_WIDTH - 1, ILI9488_HEIGHT - 1);

//	for (i = 0; i < ILI9488_WIDTH; i++)
//	{
//		for (j = 0; j < ILI9488_HEIGHT; j++)
//		{
//			ili9488_send_three_bytes(color);
//		}
//	}

    for (i = 0; i < ILI9488_BUF_SIZE / 3; i++)
    {
        ili9488_buf[3 * i] 		= (uint8_t)(color >> 16);
        ili9488_buf[3 * i + 1] 	= (uint8_t)(color >> 8);
        ili9488_buf[3 * i + 2] 	= (uint8_t)(color);
    }
    ili9488_send_data_dma(ili9488_buf, ILI9488_BUF_SIZE);
}

void ili9488_fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint32_t color)
{
    uint32_t i = 0;
    uint32_t size = 0, size_remain = 0;

    size = (x_end - x_start) * (y_end - y_start) * 3;
    if(size > ILI9488_BUF_SIZE)
    {
        size_remain = size - ILI9488_BUF_SIZE;
        size = ILI9488_BUF_SIZE;
    }

    ili9488_set_address(x_start, y_start, x_end, y_end);

//	for (i = y_start; i <= y_end; i++)
//	{
//		for (j = x_start; j <= x_end; j++)ili9488_send_three_bytes(color);
//	}
    do
    {
        for (i = 0; i < size / 3; i++)
        {
            ili9488_buf[3 * i] 		= (uint8_t)(color >> 16);
            ili9488_buf[3 * i + 1] 	= (uint8_t)(color >> 8);
            ili9488_buf[3 * i + 2] 	= (uint8_t)(color);
        }

        ili9488_send_data_dma(ili9488_buf, size);

        /* calculate the number of fill next time */
        if (size_remain > ILI9488_BUF_SIZE)
        {
            size_remain = size_remain - ILI9488_BUF_SIZE;
        }
        else
        {
            size = size_remain;
            size_remain = 0;
        }
    } while(size_remain != 0);	/* Fill completed */
}

void ili9488_fill_with_color_array(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint32_t *buf)
{
    uint32_t i = 0;
    uint32_t size = 0, size_remain = 0;

    size = (x_end - x_start) * (y_end - y_start) * 3;
    if(size > ILI9488_BUF_SIZE)
    {
        size_remain = size - ILI9488_BUF_SIZE;
        size = ILI9488_BUF_SIZE;
    }

    ili9488_set_address(x_start, y_start, x_end, y_end);

    do
    {
        for (i = 0; i < size / 3; i++)
        {
            ili9488_buf[3 * i] 		= (uint8_t)(*buf >> 16);
            ili9488_buf[3 * i + 1] 	= (uint8_t)(*buf >> 8);
            ili9488_buf[3 * i + 2] 	= (uint8_t)(*buf);
            ++buf;
        }

        ili9488_send_data_dma(ili9488_buf, size);

        /* calculate the number of fill next time */
        if (size_remain > ILI9488_BUF_SIZE)
        {
            size_remain = size_remain - ILI9488_BUF_SIZE;
        }
        else
        {
            size = size_remain;
            size_remain = 0;
        }
    } while(size_remain != 0);	/* Fill completed */
}

void ili9488_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t t;
    uint32_t i = 0;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;

    if (y1 == y2)
    {
        /* fast draw transverse line */
        ili9488_set_address(x1, y1, x2, y2);

        for (i = 0; i < x2 - x1; i++)
        {
            ili9488_buf[3 * i] = FORE_COLOR >> 16;
            ili9488_buf[3 * i + 1] = FORE_COLOR >> 8;
            ili9488_buf[3 * i + 2] = FORE_COLOR;
        }

        ili9488_send_data_dma(ili9488_buf, (x2 - x1) * 3);
        return;
    }

    delta_x = x2 - x1;
    delta_y = y2 - y1;
    row = x1;
    col = y1;
    if (delta_x > 0)incx = 1;
    else if (delta_x == 0)incx = 0;
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0;
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)distance = delta_x;
    else distance = delta_y;
    for (t = 0; t <= distance + 1; t++)
    {
        ili9488_draw_point(row, col);
        xerr += delta_x ;
        yerr += delta_y ;
        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

void ili9488_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    ili9488_draw_line(x1, y1, x2, y1);
    ili9488_draw_line(x1, y1, x1, y2);
    ili9488_draw_line(x1, y2, x2, y2);
    ili9488_draw_line(x2, y1, x2, y2);
}

void ili9488_draw_circle(uint16_t x0, uint16_t y0, uint16_t r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);
    while (a <= b)
    {
        ili9488_draw_point(x0 - b, y0 - a);
        ili9488_draw_point(x0 + b, y0 - a);
        ili9488_draw_point(x0 - a, y0 + b);
        ili9488_draw_point(x0 - b, y0 - a);
        ili9488_draw_point(x0 - a, y0 - b);
        ili9488_draw_point(x0 + b, y0 + a);
        ili9488_draw_point(x0 + a, y0 - b);
        ili9488_draw_point(x0 + a, y0 + b);
        ili9488_draw_point(x0 - b, y0 + a);
        a++;
        //Bresenham
        if (di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
        ili9488_draw_point(x0 + a, y0 + b);
    }
}

void ili9488_show_image(uint16_t x, uint16_t y, uint16_t length, uint16_t wide, const uint8_t *p)
{
    if (x + length > ILI9488_WIDTH || y + wide > ILI9488_HEIGHT)	return;

    ili9488_set_address(x, y, x + length - 1, y + wide - 1);
	ili9488_send_data_dma(p, length * wide * 3);
}
